using Xamarin.Forms;

namespace amanda.client.Infrastructure.Measuring
{
	public struct DisplayedValue
	{
		public string Value { get; }
		public Color Color { get; }
		public bool Bold { get; }
		public bool Italic { get; }
		public bool Underline { get; }

		public DisplayedValue(string value) : this(value, Color.Black) { }
		public DisplayedValue(string value, Color color) : this(value, color, false) { }
		public DisplayedValue(string value, Color color, bool bold) : this(value, color, bold, false) { }
		public DisplayedValue(string value, Color color, bool bold, bool italic) : this(value, color, bold, italic, false) { }
		public DisplayedValue(string value, Color color, bool bold, bool italic, bool underline)
		{
			Value = value;
			Color = color;
			Bold = bold;
			Italic = italic;
			Underline = underline;
		}
	}

	public interface IValueDisplay
	{
		DisplayedValue Display<T>(T obj);
		DisplayedValue Display(bool state);
		DisplayedValue Display(double value, string unit);
	}
}
