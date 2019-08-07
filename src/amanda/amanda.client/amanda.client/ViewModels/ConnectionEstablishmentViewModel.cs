using System.Threading.Tasks;
using System.Net.Http;
using Xamarin.Forms;

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

		private ConnectionEstablishmentViewModel(string name) : base(name)
		{
			address = string.Empty;
			port = "80";
			isConnecting = false;
			connect = new Command(async () => await TryConnect(), () => !IsConnecting);
		}

		private async Task TryConnect()
		{
			void StartConnecting()
			{
				IsConnecting = true;
			}
			async Task EndConnecting(string title = null, string message = null, string cancel = null)
			{
				IsConnecting = false;
				if (title == null || message == null || cancel == null) return;
				await Application.Current.MainPage.DisplayAlert(title, message, cancel);
			}

			if (string.IsNullOrWhiteSpace(address))
			{
				await Application.Current.MainPage.DisplayAlert("Error", "Please, fill in the Address of the device.", "OK");
				return;
			}

			if (string.IsNullOrWhiteSpace(port))
			{
				port = "80";
			}

			StartConnecting();
			
			var client = Dependency.Resolve<HttpClient>();
			string content = null;

			try
			{
				using (var response = await client.PostAsync("http://" + address + ":" + port + "/", Communication.Protocol.HelloMessage))
				{
					if (response.IsSuccessStatusCode)
					{
						content = await response.Content.ReadAsStringAsync();
					}
					else await EndConnecting("Error", "Failed to connect to the device.", "OK");
				}
			}
			catch
			{
				await EndConnecting("Error", "Could not establish the connection with the device.", "OK");
			}

			if (content != null)
			{
				if (content == Communication.Protocol.HelloReply)
				{
					await EndConnecting("Success", "Connection established!", "OK");
				}
				else await EndConnecting("Protocol Error", "Device did not respond with a valid message.", "OK");
			}
		}
	}
}
