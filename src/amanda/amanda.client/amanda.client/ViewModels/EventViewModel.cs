using System;

using amanda.client.Models.Events;

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

		public EventViewModel(Event e)
		{
			Event = e ?? throw new ArgumentNullException("Event cannot be null.");
			e.EventChanged += OnEventChanged;
		}
		~EventViewModel()
		{
			Event.EventChanged += OnEventChanged;
		}

		private void OnEventChanged(object sender, EventArgs e)
		{
			OnPropertyChanged(nameof(ID));
			OnPropertyChanged(nameof(Name));
		}
	}
}
