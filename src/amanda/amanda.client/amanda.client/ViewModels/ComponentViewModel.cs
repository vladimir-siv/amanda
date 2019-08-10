using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Xamarin.Forms;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;

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

		}

		private async Task SetValue()
		{

		}
	}
}
