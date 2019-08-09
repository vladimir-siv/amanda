using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.Text;
using Xamarin.Forms;
using amanda.client.Communication;

namespace amanda.client.ViewModels
{
	public sealed class ConnectionEstablishmentViewModel : ViewModel
	{
		internal static void Inject() { new ConnectionEstablishmentViewModel("ConnectionEstablishment"); }

		private string address;
		public string Address
		{
			get { return address; }
			set { address = value; OnPropertyChanged(); }
		}

		private string port;
		public string Port
		{
			get { return port; }
			set { port = value; OnPropertyChanged(); }
		}

		private bool isConnecting;
		public bool IsConnecting
		{
			get { return isConnecting; }
			set { if (isConnecting == value) return; isConnecting = value; OnPropertyChanged(); Connect.ChangeCanExecute(); }
		}

		private Command connect;
		public Command Connect { get { return connect; } }

		public event ViewModelEventHandler<ViewModelEventArgs> ConnectionEstablished;

		private ConnectionEstablishmentViewModel(string name) : base(name)
		{
			address = string.Empty;
			port = "80";
			isConnecting = false;
			connect = new Command(async () => await TryConnect(), () => !IsConnecting);
		}

		private async Task TryConnect()
		{
			if (string.IsNullOrWhiteSpace(address))
			{
				await DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Error("Please, fill in the Address of the device."));
				return;
			}

			if (string.IsNullOrWhiteSpace(port))
			{
				port = "80";
			}

			IsConnecting = true;

			var client = Dependency.Resolve<HttpClient>();
			Task result = null;
			
			try
			{
				using (var body = new StringContent(Protocol.HelloMessage, Encoding.UTF8, "application/xml"))
				{
					using (var response = await client.PostAsync("http://" + address + ":" + port + "/", body))
					{
						if (response.IsSuccessStatusCode)
						{
							var content = await response.Content.ReadAsStringAsync();

							if (content != null)
							{
								if (content == Protocol.HelloReply)
								{
									var connection = Dependency.Resolve<Connection>();
									connection.Address = address;
									connection.Port = Convert.ToInt32(port);

									result = DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Information("Success"));
								}
								else result = DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Error("Device did not respond correctly. Error may be in the network."));
							}
							else result = DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Error("Device is not responding correctly."));
						}
						else result = DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Error("Connection was not properly established with the device."));
					}
				}
			}
			catch (Exception ex)
			{
				result = DispatchAsync(ConnectionEstablished, ViewModelEventArgs.Error("Device did not respond. Reason:\r\n" + ex.Message));
			}
			
			IsConnecting = false;

			if (result != null) await result;
		}
	}
}
