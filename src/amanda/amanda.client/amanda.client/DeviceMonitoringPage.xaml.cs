﻿using System;
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
		public ObservableCollection<ComponentViewModel> Components => RemoteDevice.Components;
		
		private bool isLoading;
		public bool IsLoading
		{
			get { return isLoading; }
			set { if (value == isLoading) return; isLoading = value; OnPropertyChanged(); }
		}

		public bool HasComponents
		{
			get { return Components.Count > 0; }
			private set { OnPropertyChanged(); }
		}
		
        public DeviceMonitoringPage()
        {
            InitializeComponent();
			BindingContext = this;
        }

		protected async override void OnAppearing()
		{
			base.OnAppearing();

			if (Components.Count == 0)
			{
				IsLoading = true;

				Task alert = null;

				try
				{
					string xml_scan = await RemoteDevice.Send(Protocol.ScanHardware);
					await RemoteDevice.LoadComponents(xml_scan);
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
			}

			if (PageStack.Children.Contains(LoadingIndicator))
				PageStack.Children.Remove(LoadingIndicator);

			RemoteDevice.CollectorFailed += CollectorFailed;

			RemoteDevice.RunCollector();

			HasComponents = true;
		}

		protected override void OnDisappearing()
		{
			RemoteDevice.CollectorFailed -= CollectorFailed;
			base.OnDisappearing();
		}

		private async void CollectorFailed(RemoteDevice.CollectorFailedEventArgs e)
		{
			await DisplayAlert("Error", "Connection to the device lost. Reason:\r\n" + e.Reason + "\r\nRetrying . . .", "OK");
			RemoteDevice.RunCollector();
		}

		private async void OnItemTapped(object sender, ItemTappedEventArgs e)
        {
			ComponentViewModel item = e.Item as ComponentViewModel;
			if (item == null) return;
			await Navigation.PushAsync(new ComponentPage(item));
		}
    }
}
