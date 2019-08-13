using System;
using Xamarin.Forms;

namespace amanda.client.Infrastructure.Measuring
{
	public class DirectDisplay : IValueDisplay
	{
		private static readonly DirectDisplay instance = new DirectDisplay();
		public static DirectDisplay Resolve() { return instance; }
		private DirectDisplay() { }
		public DisplayedValue Display<T>(T obj) { return new DisplayedValue(obj.ToString(), Color.Crimson); }
		public DisplayedValue Display(bool state) { return new DisplayedValue(state.ToString(), Color.Crimson); }
		public DisplayedValue Display(double value, string unit) { return new DisplayedValue(value + " " + unit, Color.Crimson); }
	}

	public class DigitalState : IValue
	{
		public static DigitalState FromString(string str)
		{
			var state = new DigitalState();
			state.Write(str);
			return state;
		}

		public IValueDisplay Displayer { get; set; }

		public bool Value { get; protected set; }

		public event EventHandler<EventArgs> OnValueChanged;

		public DigitalState() : this(false) { }
		public DigitalState(bool value) : this(value, DirectDisplay.Resolve()) { }
		public DigitalState(bool value, IValueDisplay displayer)
		{
			Value = value;
			Displayer = displayer;
		}
		public IValue Clone() { return new DigitalState(Value, Displayer); }
		
		public bool Equals(IValue other)
		{
			if (other is DigitalState) return Value == ((DigitalState)other).Value;
			return false;
		}
		public DisplayedValue Display() { return Displayer.Display(Value); }
		public string Read() { return Value.ToString(); }
		public void Write(string value)
		{
			string lstr = value.ToLower();

			try { lstr = Convert.ToInt32(lstr).ToString(); }
			catch { }

			if
			(
				lstr == "on"
				||
				lstr == "yes"
				||
				lstr == "true"
				||
				lstr == "1"
			)
			{
				if (Value) return;
				Value = true;
				OnValueChanged?.Invoke(this, EventArgs.Empty);
				return;
			}

			if
			(
				lstr == "off"
				||
				lstr == "no"
				||
				lstr == "false"
				||
				lstr == "0"
			)
			{
				if (!Value) return;
				Value = false;
				OnValueChanged?.Invoke(this, EventArgs.Empty);
				return;
			}

			throw new FormatException("Digital value not correct.");
		}
	}

	public class AnalogValue : IValue
	{
		public static AnalogValue FromString(string str)
		{
			var value = new AnalogValue();
			value.Write(str);
			return value;
		}

		public IValueDisplay Displayer { get; set; }

		public double Value { get; protected set; }
		public string Unit { get; protected set; }

		public event EventHandler<EventArgs> OnValueChanged;

		public AnalogValue() : this(0) { }
		public AnalogValue(double value) : this(value, string.Empty) { }
		public AnalogValue(double value, string unit) : this(value, unit, DirectDisplay.Resolve()) { }
		public AnalogValue(double value, string unit, IValueDisplay displayer)
		{
			Value = value;
			Unit = unit;
			Displayer = displayer;
		}
		public IValue Clone() { return new AnalogValue(Value, Unit, Displayer); }

		public bool Equals(IValue other)
		{
			if (other is AnalogValue)
			{
				var av = (AnalogValue)other;
				return Value == av.Value && Unit == av.Unit;
			}
			return false;
		}
		public DisplayedValue Display() { return Displayer.Display(Value, Unit); }
		public string Read() { return Value + " " + Unit; }
		public void Write(string value)
		{
			string[] parts = value.Split(' ');
			if (parts.Length != 1 && parts.Length != 2) throw new FormatException("Analog value not correct.");
			Value = Convert.ToDouble(parts[0]);
			if (parts.Length == 2) Unit = parts[1];
			OnValueChanged?.Invoke(this, EventArgs.Empty);
		}
	}

	public static class ValueConverter
	{
		public static double ValueConversion(this string str)
		{
			try
			{
				var ds = DigitalState.FromString(str);
				return ds.Value ? 1.0 : 0.0;
			}
			catch { }

			try
			{
				var av = AnalogValue.FromString(str);
				return av.Value;
			}
			catch { }

			return 0.0;
		}
	}
}
