using System;
using System.Threading.Tasks;

using Xamarin.Forms;

using amanda.client.Models.Events;
using amanda.client.Communication;

namespace amanda.client.ViewModels
{
    public sealed class EventViewModel : ViewModel
    {
		public Event Event { get; }

		public uint ID
		{
			get { return Event.ID; }
			set { Event.ID = value; OnPropertyChanged(); }
		}

		public string Name
		{
			get { return Event.Name; }
			set { Event.Name = value; OnPropertyChanged(); }
		}

		public uint Repeat
		{
			get { return Event.Repeat; }
			set { Event.Repeat = value; OnPropertyChanged(); }
		}

		// Extensions

		private bool commandIssued;
		public bool CommandIssued
		{
			get { return commandIssued; }
			set { if (commandIssued == value) return; commandIssued = value; OnPropertyChanged(); Refresh.ChangeCanExecute(); Delete.ChangeCanExecute(); }
		}

		private Command refresh;
		public Command Refresh => refresh;

		private Command delete;
		public Command Delete => delete;

		public event EventHandler<EventArgs> EventChanged;
		public event Func<Task<bool>> EventDeleting;
		public event EventHandler<ViewModelEventArgs> EventDeleted;

		public EventViewModel(Event e)
		{
			Event = e ?? throw new ArgumentNullException("Event cannot be null.");
			e.EventChanged += OnEventChanged;

			commandIssued = false;
			refresh = new Command(async () => await RefreshEvents(), () => !CommandIssued);
			delete = new Command(async () => await DeleteEvent(), () => !CommandIssued);
		}
		~EventViewModel()
		{
			Event.EventChanged -= OnEventChanged;
		}

		private void OnEventChanged(object sender, EventArgs e)
		{
			OnPropertyChanged(nameof(ID));
			OnPropertyChanged(nameof(Name));
			OnPropertyChanged(nameof(Repeat));
			EventChanged?.Invoke(this, e);
		}

		private async Task RefreshEvents()
		{
			try
			{
				CommandIssued = true;
				string xml_scan = await RemoteDevice.Send(Protocol.ScanEvents);
				await RemoteDevice.LoadEvents(xml_scan);
			}
			catch { }
			finally { CommandIssued = false; }
		}

		private async Task DeleteEvent()
		{
			bool success = false;

			try
			{
				CommandIssued = true;

				bool deleteConfirmed = await EventDeleting();

				if (deleteConfirmed)
				{
					string response = await RemoteDevice.Send(Protocol.DeleteEvent(ID));
					if (success = response == Protocol.ActionSuccess) EventDeleted?.Invoke(this, ViewModelEventArgs.Information("Success"));
					else EventDeleted?.Invoke(this, ViewModelEventArgs.Error("The event was not deleted."));
				}
			}
			catch (Exception ex) { EventDeleted?.Invoke(this, ViewModelEventArgs.Error("Failed to delete the event. Reason:\r\n" + ex.Message)); }
			finally { if (!success) CommandIssued = false; }
		}
	}
}
