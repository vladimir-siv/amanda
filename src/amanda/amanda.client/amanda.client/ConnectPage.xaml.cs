using System.Threading.Tasks;
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
		}

		protected async Task OnConnectionEstablished(ViewModel sender, ViewModelEventArgs e)
		{
			if (e.MType != ViewModelEventArgs.MessageType.Error)
			{
				await DisplayAlert("Success", "Connection established!", "OK");
				await Navigation.PushAsync(new MenuPage());
			}
			else await DisplayAlert("Error", e.Message, "OK");
		}

		private async Task OnConnectionSkipped(ViewModel sender, ViewModelEventArgs e)
		{
			await Navigation.PushAsync(new MenuPage());
		}
	}
}
