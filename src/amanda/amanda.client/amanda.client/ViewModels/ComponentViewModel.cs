using System;
using System.Collections.Generic;
using Xamarin.Forms;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;

namespace amanda.client.ViewModels
{
	public sealed class ComponentViewModel : ViewModel
	{
		private Component component;

		public uint ID
		{
			get { return component.ID; }
			set { component.ID = value; OnPropertyChanged(); }
		}
		public string CType
		{
			get { return component.CType.AsProtocolString(); }
			set { component.CType = value.AsCType(); OnPropertyChanged(); }
		}
		public string Description
		{
			get { return component.Description; }
			set { component.Description = value; OnPropertyChanged(); }
		}
		public IEnumerable<string> Commands
		{
			get { return component.Commands; }
			set { component.Commands = value; OnPropertyChanged(); }
		}
		public IValue Value
		{
			get { return component.Value; }
			set
			{
				component.Value = value;
				OnPropertyChanged();
				OnValueChange(value, EventArgs.Empty);
			}
		}

		public string ValueText => Value.Display().Value;
		public Color ValueColor => Value.Display().Color;

		public ComponentViewModel(Component component)
		{
			this.component = component ?? throw new ArgumentNullException("Component cannot be null.");

			component.Value.OnValueChange += OnValueChange;
		}
		~ComponentViewModel()
		{
			component.Value.OnValueChange -= OnValueChange;
		}

		private void OnValueChange(object sender, EventArgs e)
		{
			OnPropertyChanged(nameof(ValueText));
			OnPropertyChanged(nameof(ValueColor));
		}
	}
}
