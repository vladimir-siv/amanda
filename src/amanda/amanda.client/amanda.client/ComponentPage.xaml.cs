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
		#region Command Handling

		// TODO: Abstract this in another class/view

		private TapGestureRecognizer command_tap = null;
		private void InitTap()
		{
			if (command_tap != null) return;
			command_tap = new TapGestureRecognizer();
			command_tap.Tapped += OnCommandTap;
		}

		private Label SelectedCommand = null;

		private void CreateCommand(string text)
		{
			InitTap();

			// TODO: Create a label pool in the abstraction.
			Label lbl = new Label
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

			lbl.GestureRecognizers.Add(command_tap);

			CommandList.Children.Add(lbl);
		}

		private void OnCommandTap(object sender, EventArgs e)
		{
			Label tapped = sender as Label;
			if (tapped == null) return;

			if (tapped == SelectedCommand) return;

			if (SelectedCommand != null) SelectedCommand.BackgroundColor = (Color)Resources["BackgroundColor"];
			SelectedCommand = tapped;
			SelectedCommand.BackgroundColor = (Color)Resources["SelectedColor"];
		}

		#endregion

		#region Argument Handling

		// TODO: Abstract this in another class/view

		private void OnAddArgTap(object sender, EventArgs e)
		{
			// TODO: Create an entry pool in the abstraction.
			Entry tb = new Entry
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				Placeholder = "Argument value",

				HorizontalTextAlignment = TextAlignment.Center,
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};

			ArgumentList.Children.Add(tb);
		}

		private void OnRemoveArgTap(object sender, EventArgs e)
		{
			if (ArgumentList.Children.Count > 0)
			{
				ArgumentList.Children.RemoveAt(ArgumentList.Children.Count - 1);
			}
		}

		private async void OnSendTap(object sender, EventArgs e)
		{
			if (SelectedCommand == null)
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
				string[] args = new string[ArgumentList.Children.Count];

				for (int i = 0; i < args.Length; ++i)
				{
					var entry = ArgumentList.Children[i] as Entry;
					if (entry != null) args[i] = entry.Text;
				}

				var command = Protocol.Command(vm.ID, vm.CType.AsCType(), SelectedCommand.Text, args);

				var response = await RemoteDevice.Send(command);

				if (response == Protocol.ActionSuccess) await DisplayAlert("Success", "Command successfully executed.", "OK");
				else await DisplayAlert("Error", "The device could not execute such command.", "OK");
			}
			catch (Exception ex) { await DisplayAlert("Error", "Command issuing failed. Reason:\r\n" + ex.Message, "OK"); }
			finally { btnSendCommand.IsEnabled = true; }
		}

		#endregion

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

			bool noCommands = true;

			foreach (var command in vm.Commands)
			{
				CreateCommand(command);
				noCommands = false;
			}

			if (noCommands)
			{
				StackLayout stack;

				stack = btnAddArg.Parent as StackLayout;
				if (stack != null) stack.Children.Remove(btnAddArg);

				stack = btnRemoveArg.Parent as StackLayout;
				if (stack != null) stack.Children.Remove(btnRemoveArg);

				stack = btnSendCommand.Parent as StackLayout;
				if (stack != null) stack.Children.Remove(btnSendCommand);
			}
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
