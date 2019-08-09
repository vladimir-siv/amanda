using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;
using amanda.client.ViewModels;

namespace amanda.client
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class DeviceMonitoringPage : ContentPage
    {
		#region Components

		private Component led1 = new Component
		(
			1,
			ComponentTypes.DigitalElement,
			"LED",
			new List<string>() { "blink", "stop" },
			new DigitalState(false, OnOffDisplay.Instance())
		);
		private Component led2 = new Component
		(
			2,
			ComponentTypes.DigitalElement,
			"LED",
			new List<string>() { "blink", "stop" },
			new DigitalState(true, OnOffDisplay.Instance())
		);
		private Component pir = new Component
		(
			1,
			ComponentTypes.DigitalSensor,
			"PIR",
			null,
			new DigitalState(true, YesNoDisplay.Instance())
		);
		private Component tmr = new Component
		(
			1,
			ComponentTypes.AnalogSensor,
			"Timer",
			null,
			new AnalogValue(467.32, "ms")
		);
		private Component buzz = new Component
		(
			1,
			ComponentTypes.AnalogElement,
			"BUZZ",
			null,
			new AnalogValue(8342.5, "n")
		);

		#endregion

		public ObservableCollection<ComponentViewModel> Items { get; private set; }

        public DeviceMonitoringPage()
        {
            InitializeComponent();

			Items = new ObservableCollection<ComponentViewModel>
			{
				new ComponentViewModel(led1),
				new ComponentViewModel(led2),
				new ComponentViewModel(pir),
				new ComponentViewModel(tmr),
				new ComponentViewModel(buzz)
			};
			
			BindingContext = this;
        }

        private async void OnItemTapped(object sender, ItemTappedEventArgs e)
        {
			ComponentViewModel item = e.Item as ComponentViewModel;
			if (item == null) return;

			await Task.Delay(1500);

			if (item.Value is AnalogValue)
			{
				Random r = new Random();
				double val = r.NextDouble() * 1000.0;
				item.Value.Write(string.Format("{0:F2} lux", val));
				return;
			}

			if (item.Value is DigitalState)
			{
				Random r = new Random();
				bool state = r.Next(2) == 1;
				item.Value.Write(state.ToString());
				return;
			}
        }
    }
}
