using Xamarin.Forms;
using amanda.client.ViewModels;

namespace amanda.client
{
	public partial class ConnectPage : ContentPage
	{
		public ConnectPage()
		{
			InitializeComponent();

			BindingContext = ViewModel.Resolve("ConnectionEstablishment");
		}
	}
}
