using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Xamarin.Forms;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;
using amanda.client.Communication;

namespace amanda.client.ViewModels
{
	public sealed class ComponentViewModel : ViewModel
	{
		private Component Component { get; }

		public uint ID
		{
			get { return Component.ID; }
			set { Component.ID = value; OnPropertyChanged(); }
		}
		public string CType
		{
			get { return Component.CType.AsProtocolString(); }
			set { Component.CType = value.AsCType(); OnPropertyChanged(); }
		}
		public string Description
		{
			get { return Component.Description; }
			set { Component.Description = value; OnPropertyChanged(); }
		}
		public IEnumerable<string> Commands
		{
			get { return Component.Commands; }
			set { Component.Commands = value; OnPropertyChanged(); }
		}
		public IValue Value
		{
			get { return Component.Value; }
			set
			{
				Component.Value = value;
				OnPropertyChanged();
				OnValueChange(value, EventArgs.Empty);
			}
		}

		public string ValueText => Value.Display().Value;
		public Color ValueColor => Value.Display().Color;

		// Extensions

		private bool commandIssued;
		public bool CommandIssued
		{
			get { return commandIssued; }
			set { if (commandIssued == value) return; commandIssued = value; OnPropertyChanged(); Toggle.ChangeCanExecute(); Set.ChangeCanExecute(); }
		}

		private string functionArgument;
		public string FunctionArgument
		{
			get { return functionArgument; }
			set { functionArgument = value; OnPropertyChanged(); }
		}

		private Command toggle;
		public Command Toggle => toggle;

		private Command set;
		public Command Set => set;

		public event ViewModelEventHandler<ViewModelEventArgs> FunctionExecuted;

		public ComponentViewModel(Component component)
		{
			Component = component ?? throw new ArgumentNullException("Component cannot be null.");
			component.Value.OnValueChange += OnValueChange;

			commandIssued = false;
			functionArgument = string.Empty;
			toggle = new Command(async () => await ToggleValue(), () => !CommandIssued);
			set = new Command(async () => await SetValue(), () => !CommandIssued);
		}
		~ComponentViewModel()
		{
			Component.Value.OnValueChange -= OnValueChange;
		}

		private void OnValueChange(object sender, EventArgs e)
		{
			OnPropertyChanged(nameof(ValueText));
			OnPropertyChanged(nameof(ValueColor));
		}

		private async Task ToggleValue()
		{
			if (!Component.CType.IsAny(Models.Components.CType.Digital)) return;

			if (!(Component.Value is DigitalState)) return;

			try
			{
				CommandIssued = true;

				var val = (DigitalState)Component.Value;

				var result = await RemoteDevice.Send(Protocol.IODigitalWrite(ID, !val.Value));

				ViewModelEventArgs e;
				if (result == Protocol.ActionSuccess) e = ViewModelEventArgs.Information("Success");
				else e = ViewModelEventArgs.Error("Toggling failed.");

				FunctionExecuted?.Invoke(this, e);
			}
			catch { }
			finally { CommandIssued = false; }
		}

		private async Task SetValue()
		{
			if (!Component.CType.IsAny(Models.Components.CType.Analog)) return;

			if (!(Component.Value is AnalogValue)) return;

			try
			{
				CommandIssued = true;

				double av = Convert.ToDouble(FunctionArgument);

				var val = (AnalogValue)Component.Value;

				var result = await RemoteDevice.Send(Protocol.IOAnalogWrite(ID, av, val.Unit));

				ViewModelEventArgs e;
				if (result == Protocol.ActionSuccess) e = ViewModelEventArgs.Information("Success");
				else e = ViewModelEventArgs.Error("Failed to set the value.");

				FunctionExecuted?.Invoke(this, e);
			}
			catch { }
			finally { CommandIssued = false; }
		}
	}
}
