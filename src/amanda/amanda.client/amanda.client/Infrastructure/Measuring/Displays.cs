﻿using Xamarin.Forms;

using amanda.client.Models.Components;

namespace amanda.client.Infrastructure.Measuring
{
	public class OnOffDisplay : IValueDisplay
	{
		private static readonly OnOffDisplay instance = new OnOffDisplay();
		public static OnOffDisplay Resolve() { return instance; }
		private OnOffDisplay() { }
		public DisplayedValue Display<T>(T obj)
		{
			return obj != null ?
				new DisplayedValue("ON", Color.Green)
				:
				new DisplayedValue("OFF", Color.Red);
		}
		public DisplayedValue Display(bool state)
		{
			return state ?
				new DisplayedValue("ON", Color.Green)
				:
				new DisplayedValue("OFF", Color.Red);
		}
		public DisplayedValue Display(double value, string unit)
		{
			return value != 0.0 ?
				new DisplayedValue("ON", Color.Green)
				:
				new DisplayedValue("OFF", Color.Red);
		}
	}

	public class YesNoDisplay : IValueDisplay
	{
		private static readonly YesNoDisplay instance = new YesNoDisplay();
		public static YesNoDisplay Resolve() { return instance; }
		private YesNoDisplay() { }
		public DisplayedValue Display<T>(T obj)
		{
			return obj != null ?
				new DisplayedValue("YES", Color.Cyan)
				:
				new DisplayedValue("NO", Color.Brown);
		}
		public DisplayedValue Display(bool state)
		{
			return state ?
				new DisplayedValue("YES", Color.Cyan)
				:
				new DisplayedValue("NO", Color.Brown);
		}
		public DisplayedValue Display(double value, string unit)
		{
			return value != 0.0 ?
				new DisplayedValue("YES", Color.Cyan)
				:
				new DisplayedValue("NO", Color.Brown);
		}
	}

	public static class Displays
	{
		public static IValueDisplay ResolveDisplay(this CType ctype)
		{
			// [NOTE] This method may be suitable to have a string as a second parameter,
			// which is actually a component description (led, pir, button, ...), if
			// necessary to distinguish certain components
			if (ctype == ComponentTypes.DigitalSensor) return YesNoDisplay.Resolve();
			if (ctype == ComponentTypes.DigitalElement) return OnOffDisplay.Resolve();
			return DirectDisplay.Resolve();
		}
	}
}
