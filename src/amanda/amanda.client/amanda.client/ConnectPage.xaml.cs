using Xamarin.Forms;

namespace amanda.client
{
	public partial class ConnectPage : ContentPage
	{
		public ConnectPage()
		{
			InitializeComponent();

			BindingContext = ViewModels.ViewModel.Resolve("ConnectionEstablishment");
		}
	}
}
