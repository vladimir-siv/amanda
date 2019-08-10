using System;
using System.Linq;
using System.Collections.ObjectModel;
using System.Net.Http;
using System.Threading.Tasks;
using System.Text;
using System.Xml;

using Xamarin.Forms;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;
using amanda.client.ViewModels;

namespace amanda.client.Communication
{
    public static class Protocol
    {
		public const double RefreshSpeed = 2500;

		public const string HelloMessage = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/hello\"></action>";
		public const string HelloReply = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><message>Hello World.</message>";
		public const string TimeMessage = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>1:AS</arg><arg>read</arg></action>";
		public const string ScanHardware = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/hardware\"></action>";
		public const string ScanEvents = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/events\"></action>";
	}

	public static class RemoteDevice
	{
		public static ObservableCollection<ComponentViewModel> Components { get; } = new ObservableCollection<ComponentViewModel>();

		public class CollectorFailedEventArgs : EventArgs
		{
			public string Reason { get; }
			public CollectorFailedEventArgs(string reason) { Reason = reason; }
		}

		private static bool isCollecting = false;
		private static bool collect { get; set; }
		
		public static void RunCollector()
		{
			collect = true;

			if (isCollecting) return;

			isCollecting = true;
			
			Device.StartTimer(TimeSpan.FromMilliseconds(Protocol.RefreshSpeed), () =>
			{
				if (collect)
				{
					try
					{
						Task.Run(async () =>
						{
							string xml_scan = await Send(Protocol.ScanHardware);
							await LoadComponents(xml_scan);
						}).Wait();
					}
					catch (Exception ex)
					{
						collect = false;
						isCollecting = false;
						CollectorFailed?.Invoke(new CollectorFailedEventArgs(ex.Message));
						return false;
					}
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

		public static async Task LoadComponents(string xml_scan)
		{
			await Task.Run(() =>
			{
				var doc = new XmlDocument();
				doc.LoadXml(xml_scan);

				var root = doc.ChildNodes[1];
				if (root == null || root.Name != "scan") throw new XmlException("Invalid scan format.");

				foreach (XmlNode node in root.ChildNodes)
				{
					if (node.Name != "component") throw new XmlException("Invalid scan format.");

					try
					{
						uint vid = Convert.ToUInt32(node.Attributes["vid"].Value);
						CType ctype = node.Attributes["ctype"].Value.AsCType();
						string description = node.Attributes["description"].Value;
						string[] commands = node.Attributes["commands"].Value.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);

						IValue value = null;

						var vnode = node.FirstChild;
						if (vnode == null) throw new XmlException("Invalid scan format.");

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
			});
		}
	}
}
