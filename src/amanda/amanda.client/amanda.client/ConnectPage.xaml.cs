using System;

using Xamarin.Forms;

using amanda.client.ViewModels;

namespace amanda.client
{
	public partial class ConnectPage : ContentPage
	{
		public ConnectPage()
		{
			InitializeComponent();
		}

		protected override void OnAppearing()
		{
			base.OnAppearing();

			ViewModel vm = ViewModel.Resolve("ConnectionEstablishment");
			vm.Attach<ViewModelEventArgs>("ConnectionEstablished", OnConnectionEstablished);
			vm.Attach<ViewModelEventArgs>("SkipConnection", OnConnectionSkipped);
			BindingContext = vm;
		}

		protected override void OnDisappearing()
		{
			ViewModel vm = ViewModel.Resolve("ConnectionEstablishment");
			vm.Detach<ViewModelEventArgs>("ConnectionEstablished", OnConnectionEstablished);
			vm.Detach<ViewModelEventArgs>("SkipConnection", OnConnectionSkipped);

			base.OnDisappearing();
		}

		protected async void OnConnectionEstablished(ViewModel sender, ViewModelEventArgs e)
		{
			if (e.MType != ViewModelEventArgs.MessageType.Error)
			{
				await DisplayAlert("Success", "Connection established!", "OK");
				await Navigation.PushAsync(new MenuPage());
			}
			else await DisplayAlert("Error", e.Message, "OK");
		}

		private async void OnConnectionSkipped(ViewModel sender, ViewModelEventArgs e)
		{
			var vm = BindingContext as ConnectionEstablishmentViewModel;
			if (vm == null) throw new ApplicationException("Unexpected error occured. Please, restart the application.");

			if (vm.HasConnection) await Navigation.PushAsync(new MenuPage());
			else await DisplayAlert("Error", "No connections were previously established.", "OK");
		}
	}
}
