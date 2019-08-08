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

			ViewModel vm = ViewModel.Resolve("ConnectionEstablishment");
			vm.Attach<ViewModelEventArgs>("ConnectionEstablished", OnConnectionEstablished);
			BindingContext = vm;
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
	}
}
