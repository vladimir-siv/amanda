using Xamarin.Forms;

namespace amanda.client.Infrastructure.Measuring
{
	public class OnOffDisplay : IValueDisplay
	{
		private static readonly OnOffDisplay instance = new OnOffDisplay();
		public static OnOffDisplay Instance() { return instance; }
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
		public static YesNoDisplay Instance() { return instance; }
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
}
