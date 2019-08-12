using System;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;
using amanda.client.Models.Components;
using amanda.client.ViewModels;
using amanda.client.Extensions;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class ComponentPage : ContentPage
	{
		private Label CreateLabel(string text)
		{
			// TODO: Create a label pool.
			return new Label
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				HorizontalTextAlignment = TextAlignment.Center,
				VerticalTextAlignment = TextAlignment.Center,
				Text = text,

				BackgroundColor = (Color)Resources["BackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}
		private Entry CreateEntry()
		{
			// TODO: Create an entry pool in the abstraction.
			return new Entry
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				Placeholder = "Argument value",

				HorizontalTextAlignment = TextAlignment.Center,
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}
		
		public ComponentPage(ComponentViewModel vm)
		{
			InitializeComponent();
			BindingContext = vm;

			CType ctype = vm.CType.AsCType();

			if (ctype.IsAny(CType.Element))
			{
				if (ctype.IsAny(CType.Digital)) DigitalFunctions.IsVisible = true;
				if (ctype.IsAny(CType.Analog)) AnalogFunctions.IsVisible = true;
			}

			if (!DigitalFunctions.IsVisible) PageStack.Children.Remove(DigitalFunctions);
			if (!AnalogFunctions.IsVisible) PageStack.Children.Remove(AnalogFunctions);

			bool noCommands = true;

			foreach (var command in vm.Commands)
			{
				CommandList.Add(CreateLabel(command));
				noCommands = false;
			}

			if (noCommands)
			{
				ArgumentList.UserControllable = false;
				StackLayout stack = btnSendCommand.Parent as StackLayout;
				if (stack != null) stack.Children.Remove(btnSendCommand);
			}
			else vm.ViewGenerator = CreateEntry;
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
		
		private async void OnSendTap(object sender, EventArgs e)
		{
			if (CommandList.Selected == null)
			{
				await DisplayAlert("Error", "Please, select a command.", "OK");
				return;
			}

			var vm = BindingContext as ComponentViewModel;

			if (vm == null)
			{
				await DisplayAlert("Error", "Unexpected error occured. Try refreshing the page (go back and forth one or twice...).", "OK");
				return;
			}

			btnSendCommand.IsEnabled = false;

			try
			{
				string[] args = new string[ArgumentList.Count];
				
				for (int i = 0; i < args.Length; ++i)
				{
					var entry = ArgumentList[i] as Entry;
					if (entry != null) args[i] = entry.Text;
				}

				var command = Protocol.Command(vm.ID, vm.CType.AsCType(), ((Label)CommandList.Selected).Text, args);

				var response = await RemoteDevice.Send(command);

				if (response == Protocol.ActionSuccess) await DisplayAlert("Success", "Command successfully executed.", "OK");
				else await DisplayAlert("Error", "The device could not execute such command.", "OK");
			}
			catch (Exception ex) { await DisplayAlert("Error", "Command issuing failed. Reason:\r\n" + ex.Message, "OK"); }
			finally { btnSendCommand.IsEnabled = true; }
		}
	}
}
