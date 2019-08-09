using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class MenuPage : ContentPage
	{
		public MenuPage()
		{
			InitializeComponent();
		}

		private async void DeviceMonitoringTapped(object sender, EventArgs e)
		{
			await DisplayAlert("Error", "Device monitoring not yet implemented.", "OK");
		}

		private async void EventManagementTapped(object sender, EventArgs e)
		{
			await DisplayAlert("Error", "Event management not yet implemented.", "OK");
		}

		private async void AppSettingsTapped(object sender, EventArgs e)
		{
			await DisplayAlert("Error", "Settings not yet implemented.", "OK");
		}
	}
}
