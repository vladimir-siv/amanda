﻿using System;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class MenuPage : ContentPage
	{
		public MenuPage()
		{
			InitializeComponent();
		}

		protected override void OnAppearing()
		{
			RemoteDevice.PauseCollector();
		}

		private async void DeviceMonitoringTapped(object sender, EventArgs e)
		{
			await Navigation.PushAsync(new DeviceMonitoringPage());
		}

		private async void EventManagementTapped(object sender, EventArgs e)
		{
			await Navigation.PushAsync(new EventManagementPage());
		}

		private async void AppSettingsTapped(object sender, EventArgs e)
		{
			await DisplayAlert("Error", "Settings not yet implemented.", "OK");
		}
	}
}
