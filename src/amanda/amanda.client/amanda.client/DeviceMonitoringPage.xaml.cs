using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.ViewModels;
using amanda.client.Communication;

namespace amanda.client
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class DeviceMonitoringPage : ContentPage
    {
		public ObservableCollection<ComponentViewModel> Items { get; private set; } = new ObservableCollection<ComponentViewModel>();
		public bool IsLoading
		{
			get { return isLoading; }
			set { if (value == isLoading) return; isLoading = value; OnPropertyChanged(); }
		}
		
		private bool isLoading;

        public DeviceMonitoringPage()
        {
            InitializeComponent();
			
			BindingContext = this;

			var con = Dependency.Resolve<Connection>();
			con.Address = "192.168.56.177";
			con.Port = 80;
        }

		protected async override void OnAppearing()
		{
			base.OnAppearing();

			IsLoading = true;

			Task alert = null;

			try
			{
				string xml_scan = await RemoteDevice.Send(Protocol.ScanHardware);
				await Protocol.LoadComponents(Items, xml_scan);
			}
			catch (Exception ex)
			{
				alert = DisplayAlert("Error", "Could not load the components. Reason:\r\n" + ex.Message, "OK");
				return;
			}
			finally
			{
				IsLoading = false;
				if (alert != null) await alert;
			}

			PageStack.Children.Remove(LoadingIndicator);
			ComponentView.IsVisible = true;

			StartRefreshing();
		}

		private void StartRefreshing()
		{
			Device.StartTimer(TimeSpan.FromMilliseconds(Protocol.RefreshSpeed), () =>
			{
				try
				{
					Task.Run(async () =>
					{
						string xml_scan = await RemoteDevice.Send(Protocol.ScanHardware);
						await Protocol.LoadComponents(Items, xml_scan);
					});
				}
				catch (Exception ex)
				{
					Task.Run(async () => await DisplayAlert("Error", "Connection with the device lost. Reason:\r\n" + ex.Message, "OK"));
					return false;
				}

				return true;
			});
		}

        private async void OnItemTapped(object sender, ItemTappedEventArgs e)
        {
			
        }
    }
}
