using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.ViewModels;
using amanda.client.Communication;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class EventManagementPage : ContentPage
	{
		public ObservableCollection<EventViewModel> Events => RemoteDevice.Events;

		private bool CommandIssued
		{
			set
			{
				btnRefresh.IsEnabled = btnNew.IsEnabled = !value;
			}
		}

		private bool isLoading;
		public bool IsLoading
		{
			get { return isLoading; }
			set { if (value == isLoading) return; isLoading = value; OnPropertyChanged(); }
		}

		public EventManagementPage()
		{
			InitializeComponent();
			BindingContext = this;
		}

		protected async override void OnAppearing()
		{
			base.OnAppearing();

			if (Events.Count == 0)
			{
				Task alert = null;

				try
				{
					IsLoading = true;
					CommandIssued = true;

					string xml_scan;

					if (RemoteDevice.Components.Count == 0)
					{
						xml_scan = await RemoteDevice.Send(Protocol.ScanHardware);
						await RemoteDevice.LoadComponents(xml_scan);
					}

					xml_scan = await RemoteDevice.Send(Protocol.ScanEvents);
					await RemoteDevice.LoadEvents(xml_scan);

					CommandIssued = false;
				}
				catch (Exception ex)
				{
					alert = DisplayAlert("Error", "Could not load the events. Reason:\r\n" + ex.Message, "OK");
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

			EventView.IsVisible = true;
		}

		private async void OnItemTapped(object sender, ItemTappedEventArgs e)
		{
			EventViewModel item = e.Item as EventViewModel;
			if (item == null) return;
			await Navigation.PushAsync(new EventPage(item));
		}

		private async void OnRefreshTapped(object sender, EventArgs e)
		{
			try
			{
				CommandIssued = true;
				string xml_scan = await RemoteDevice.Send(Protocol.ScanEvents);
				await RemoteDevice.LoadEvents(xml_scan);
			}
			catch (Exception ex) { await DisplayAlert("Error", "Could not refresh events. Reason: \r\n" + ex.Message, "OK"); }
			finally { CommandIssued = false; }
		}
		private async void OnNewTapped(object sender, EventArgs e)
		{
			try
			{
				CommandIssued = true;
				await Navigation.PushAsync(new CreateEventPage());
			}
			finally { CommandIssued = false; }
		}
	}
}
