using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;
using amanda.client.Models.Components;
using amanda.client.ViewModels;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class ComponentPage : ContentPage
	{
		public ComponentPage(ComponentViewModel vm)
		{
			InitializeComponent();
			BindingContext = vm;

			CType ctype = vm.CType.AsCType();

			if (ctype.IsAny(CType.Element))
			{
				if (ctype.IsAny(CType.Digital)) DigitalFunctions.View.IsVisible = true;
				if (ctype.IsAny(CType.Analog)) AnalogFunctions.View.IsVisible = true;
			}

			if (!DigitalFunctions.View.IsVisible) ComponentFunctions.Remove(DigitalFunctions);
			if (!AnalogFunctions.View.IsVisible) ComponentFunctions.Remove(AnalogFunctions);
		}

		protected override void OnAppearing()
		{
			base.OnAppearing();
			RemoteDevice.CollectorFailed += CollectorFailed;
			RemoteDevice.RunCollector();
		}

		protected override void OnDisappearing()
		{
			RemoteDevice.PauseCollector();
			RemoteDevice.CollectorFailed -= CollectorFailed;
			base.OnDisappearing();
		}

		private async void CollectorFailed(RemoteDevice.CollectorFailedEventArgs e)
		{
			await DisplayAlert("Error", "Connection to the device lost. Reason:\r\n" + e.Reason + "\r\nRetrying . . .", "OK");
			RemoteDevice.RunCollector();
		}
	}
}
