using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;
using System.Text;
using System.Xml;

using Xamarin.Forms;

using amanda.client.Models.Components;
using amanda.client.Models.Events;
using amanda.client.Infrastructure.Measuring;
using amanda.client.ViewModels;
using amanda.client.Synchronization;

namespace amanda.client.Communication
{
	using Condition = Models.Events.Condition;

	public static class Protocol
	{
		public const double RefreshSpeed = 2500;

		public const string HelloMessage = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/hello\"></action>";
		public const string HelloReply = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><message>Hello World.</message>";
		public const string TimeMessage = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>1:AS</arg><arg>read</arg></action>";
		public const string ScanHardware = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/hardware\"></action>";
		public const string ScanEvents = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/events\"></action>";

		public static string IODigitalRead(uint id)
		{
			const string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>{0}:DS</arg><arg>read</arg></action>";
			return string.Format(xml, id);
		}
		public static string IODigitalWrite(uint id, bool state)
		{
			const string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>{0}:DE</arg><arg>write</arg><arg>{1}</arg></action>";
			return string.Format(xml, id, state ? 1 : 0);
		}
		public static string IOAnalogRead(uint id)
		{
			string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>{0}:AS</arg><arg>read</arg></action>";
			return string.Format(xml, id);
		}
		public static string IOAnalogWrite(uint id, double value, string unit)
		{
			string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>{0}:AE</arg><arg>write</arg><arg>{1:F2} {2}</arg></action>";
			return string.Format(xml, id, value, unit ?? "n");
		}

		public static string Command(uint id, CType ctype, string name, params string[] args)
		{
			const string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Cmd/exec\"><arg>{0}:{1}</arg><arg>{2}</arg>{3}</action>";

			var arguments = new StringBuilder();

			if (args != null)
			{
				foreach (var arg in args)
				{
					arguments.Append("<arg>");
					arguments.Append(arg ?? string.Empty);
					arguments.Append("</arg>");
				}
			}

			return string.Format(xml, id, ctype.AsProtocolString(), name ?? string.Empty, arguments.ToString());
		}

		public static string CreateEvent(Event e)
		{
			StringBuilder xml = new StringBuilder();
			xml.AppendFormat("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action process=\"Event/create:{0}\"><event repeat=\"{1}\">", e.Name, e.Repeat);

			xml.Append("<requirements>");
			foreach (var req in e.Requirements)
			{
				xml.Append("<pack>");
				foreach (var cond in req.Conditions)
				{
					xml.AppendFormat("<condition vid=\"{0}\" ctype=\"{1}\">", cond.ID, cond.CType.AsProtocolString());
					foreach (var cmp in cond.Comparators)
					{
						xml.AppendFormat("<{0}>{1:F2}</{0}>", cmp.Name, cmp.Value);
					}
					xml.Append("</condition>");
				}
				xml.Append("</pack>");
			}
			xml.Append("</requirements>");

			xml.Append("<actions>");
			{
				xml.Append("<raise>");
				foreach (var write in e.Raise)
				{
					xml.AppendFormat("<write vid=\"{0}\" ctype=\"{1}\">", write.ID, write.CType.AsProtocolString());

					if (write.Value == null) xml.Append("<unknown></unknown>");
					else if (write.Value is DigitalState) xml.AppendFormat("<state>{0}</state>", Convert.ToInt32(((DigitalState)write.Value).Value));
					else if (write.Value is AnalogValue) xml.AppendFormat("<value unit=\"{0}\">{1:F2}</value>", ((AnalogValue)write.Value).Unit, ((AnalogValue)write.Value).Value);
					else xml.Append("<unknown></unknown>");

					xml.Append("</write>");
				}
				xml.Append("</raise>");

				xml.Append("<expire>");
				foreach (var write in e.Expire)
				{
					xml.AppendFormat("<write vid=\"{0}\" ctype=\"{1}\">", write.ID, write.CType.AsProtocolString());

					xml.Append("</write>");
				}
				xml.Append("</expire>");
			}
			xml.Append("</actions>");

			xml.Append("</event></action>");
			return xml.ToString();
		}
		public static string DeleteEvent(uint id)
		{
			const string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Event/delete\"><arg>{0}</arg></action>";
			return string.Format(xml, id);
		}

		public const string ActionSuccess = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action>success</action>";
		public const string ActionFailed = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action>failure</action>";
	}

	public static class RemoteDevice
	{
		public class CollectorFailedEventArgs : EventArgs
		{
			public string Reason { get; }
			public CollectorFailedEventArgs(string reason) { Reason = reason; }
		}

		public static ObservableCollection<ComponentViewModel> Components { get; } = new ObservableCollection<ComponentViewModel>();
		public static ObservableCollection<EventViewModel> Events { get; } = new ObservableCollection<EventViewModel>();

		private static bool started = false;
		private static bool collect = false;
		private static Atomic<bool> cansend = new Atomic<bool>(false);
		
		public static void RunCollector()
		{
			collect = true;

			if (started) return;
			started = true;

			cansend.Value = true;

			Device.StartTimer(TimeSpan.FromMilliseconds(Protocol.RefreshSpeed), () =>
			{
				if (collect && cansend.CompareAndSet(true, false))
				{
					Task.Run(async () =>
					{
						try
						{
							string xml_scan = await Send(Protocol.ScanHardware);
							await LoadComponents(xml_scan);
						}
						catch (Exception ex)
						{
							collect = false;
							Device.BeginInvokeOnMainThread(() => CollectorFailed?.Invoke(new CollectorFailedEventArgs(ex.Message)));
						}
						finally
						{
							cansend.Value = true;
						}
					});
				}

				return true;
			});
		}
		public static void PauseCollector()
		{
			collect = false;
		}
		public static event Action<CollectorFailedEventArgs> CollectorFailed;

		public static async Task<string> Send(string xml)
		{
			var client = Dependency.Resolve<HttpClient>();
			Connection con = Dependency.Resolve<Connection>();

			using (var body = new StringContent(xml, Encoding.UTF8, "application/xml"))
			{
				using (var response = await client.PostAsync("http://" + con.Address + ":" + con.Port + "/", body))
				{
					if (response.IsSuccessStatusCode)
					{
						var content = await response.Content.ReadAsStringAsync();

						if (content != null)
						{
							return content;
						}
					}
				}
			}

			return string.Empty;
		}
		
		private static object LCsync = new object();
		public static async Task LoadComponents(string xml_scan)
		{
			await Task.Run(() =>
			{
				lock (LCsync)
				{
					var doc = new XmlDocument();
					doc.LoadXml(xml_scan);

					var root = doc.ChildNodes[1];
					if (root == null || root.Name != "scan") throw new XmlException("Invalid scan format.");

					foreach (XmlNode node in root.ChildNodes)
					{
						try
						{
							if (node.Name != "component") throw new XmlException("Invalid scan format.");
							
							var vnode = node.FirstChild;
							if (vnode == null) throw new XmlException("Invalid scan format.");

							uint vid = Convert.ToUInt32(node.Attributes["vid"].Value);
							CType ctype = node.Attributes["ctype"].Value.AsCType();
							string description = node.Attributes["description"].Value;
							string[] commands = node.Attributes["commands"].Value.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);

							IValue value = null;

							switch (vnode.Name)
							{
								case "state":
								{
									bool state = Convert.ToBoolean(Convert.ToInt32(vnode.InnerText));
									value = new DigitalState(state, ctype.ResolveDisplay());
								} break;
								case "value":
								{
									string unit = vnode.Attributes["unit"].Value;
									double analogValue = Convert.ToDouble(vnode.InnerText);
									value = new AnalogValue(analogValue, unit, ctype.ResolveDisplay());
								} break;
								default: throw new XmlException("Invalid scan format.");
							}

							// TODO: Somehow optimize this search (maybe it's not even possible - at least not in an ok way)
							var search = Components.FirstOrDefault(c => c.ID == vid && c.CType.AsCType() == ctype);

							if (search != null) search.Value.Write(value.Read());
							else Components.Add(new ComponentViewModel(new Component(vid, ctype, description, commands, value)));
						}
						catch { /* if one component is invalid, skip that one and continue on */ }
					}
				}
			});
		}
		
		private static object LEsync = new object();
		public static async Task LoadEvents(string xml_scan)
		{
			await Task.Run(() =>
			{
				lock (LEsync)
				{
					var ids = new HashSet<uint>();

					var doc = new XmlDocument();
					doc.LoadXml(xml_scan);

					var root = doc.ChildNodes[1];
					if (root == null || root.Name != "scan") throw new XmlException("Invalid scan format.");

					foreach (XmlNode ehNode in root.ChildNodes)
					{
						Event e = null;

						try
						{
							if (ehNode.Name != "event_handle") throw new XmlException("Invalid scan format.");

							uint id = Convert.ToUInt32(ehNode.Attributes["id"].Value);
							string name = ehNode.Attributes["name"].Value;

							ids.Add(id);

							// TODO: Somehow optimize this search (maybe it's not even possible - at least not in an ok way)
							var search = Events.FirstOrDefault(evm => evm.ID == id);

							if (search != null)
							{
								e = search.Event;
								e.Clean();
							}
							else
							{
								e = new Event(id, name);
								Events.Add(new EventViewModel(e));
							}
							
							var eNode = ehNode.FirstChild;
							if (eNode == null || eNode.Name != "event") throw new XmlException("Invalid scan format.");

							uint repeat = Convert.ToUInt32(eNode.Attributes["repeat"].Value);
							e.Repeat = repeat;

							var rNode = eNode.ChildNodes[0];
							if (rNode == null || rNode.Name != "requirements") throw new XmlException("Invalid scan format.");

							var aNode = eNode.ChildNodes[1];
							if (aNode == null || aNode.Name != "actions") throw new XmlException("Invalid scan format.");

							var uNode = aNode.ChildNodes[0];
							if (uNode == null || uNode.Name != "raise") throw new XmlException("Invalid scan format.");

							var dNode = aNode.ChildNodes[1];
							if (dNode == null || dNode.Name != "expire") throw new XmlException("Invalid scan format.");

							foreach (XmlNode pNode in rNode.ChildNodes)
							{
								if (pNode.Name != "pack") throw new XmlException("Invalid scan format.");

								var req = new Requirement();
								e.Requirements.AddLast(req);

								foreach (XmlNode cNode in pNode.ChildNodes)
								{
									if (cNode.Name != "condition") throw new XmlException("Invalid scan format.");

									uint vid = Convert.ToUInt32(cNode.Attributes["vid"].Value);
									CType ctype = cNode.Attributes["ctype"].Value.AsCType();

									var cond = new Condition(vid, ctype);
									req.Conditions.AddLast(cond);

									foreach (XmlNode node in cNode.ChildNodes)
									{
										var cmp = new Comparator(node.Name, Convert.ToDouble(node.InnerText));
										cond.Comparators.AddLast(cmp);
									}
								}
							}

							foreach (XmlNode wNode in uNode.ChildNodes)
							{
								if (wNode.Name != "write") throw new XmlException("Invalid scan format");
								
								var vNode = wNode.FirstChild;
								if (vNode == null) throw new XmlException("Invalid scan format.");

								uint vid = Convert.ToUInt32(wNode.Attributes["vid"].Value);
								CType ctype = wNode.Attributes["ctype"].Value.AsCType();
								IValue value = null;

								switch (vNode.Name)
								{
									case "state":
									{
										bool state = Convert.ToBoolean(Convert.ToInt32(vNode.InnerText));
										value = new DigitalState(state, ctype.ResolveDisplay());
									} break;
									case "value":
									{
										string unit = vNode.Attributes["unit"].Value;
										double analogValue = Convert.ToDouble(vNode.InnerText);
										value = new AnalogValue(analogValue, unit, ctype.ResolveDisplay());
									} break;
									default: throw new XmlException("Invalid scan format.");
								}

								e.Raise.AddLast(new Write(vid, ctype, value));
							}

							foreach (XmlNode wNode in dNode.ChildNodes)
							{
								if (wNode.Name != "write") throw new XmlException("Invalid scan format");
								
								var vNode = wNode.FirstChild;
								if (vNode == null) throw new XmlException("Invalid scan format.");

								uint vid = Convert.ToUInt32(wNode.Attributes["vid"].Value);
								CType ctype = wNode.Attributes["ctype"].Value.AsCType();
								IValue value = null;

								switch (vNode.Name)
								{
									case "state":
									{
										bool state = Convert.ToBoolean(Convert.ToInt32(vNode.InnerText));
										value = new DigitalState(state, ctype.ResolveDisplay());
									} break;
									case "value":
									{
										string unit = vNode.Attributes["unit"].Value;
										double analogValue = Convert.ToDouble(vNode.InnerText);
										value = new AnalogValue(analogValue, unit, ctype.ResolveDisplay());
									} break;
									default: throw new XmlException("Invalid scan format.");
								}

								e.Expire.AddLast(new Write(vid, ctype, value));
							}
						}
						catch { e?.Clean(); /* if one event is invalid, skip that one and continue on */ }
						finally { e?.OnEventChanged(); }
					}

					foreach (var e in Events)
					{
						if (!ids.Contains(e.ID)) Events.Remove(e);
					}
				}
			});
		}

		public static ComponentViewModel FindComponent(uint id, CType ctype)
		{
			return Components.FirstOrDefault(c => c.ID == id && c.CType.AsCType() == ctype);
		}
	}
}
