using System;
using System.Collections.Generic;
using System.Text;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;

namespace amanda.client.Models.Events
{
    public class Event
    {
		public uint ID { get; set; } = 0;
		public string Name { get; set; } = string.Empty;
		public uint Repeat { get; set; } = 0;

		public LinkedList<Requirement> Requirements { get; set; } = new LinkedList<Requirement>();
		public LinkedList<Write> Raise { get; set; } = new LinkedList<Write>();
		public LinkedList<Write> Expire { get; set; } = new LinkedList<Write>();

		public event EventHandler<EventArgs> EventChanged;

		public Event(uint id, string name = "@unnamed")
		{
			ID = id;
			Name = name;
		}

		public void OnEventChanged() { EventChanged?.Invoke(this, EventArgs.Empty); }

		public void Clean()
		{
			Repeat = 0;

			Requirements.Clear();
			Raise.Clear();
			Expire.Clear();
		}
    }

	public class Requirement // this is the Pack
	{
		public LinkedList<Condition> Conditions { get; set; } = new LinkedList<Condition>();
	}

	public class Condition
	{
		public uint ID { get; set; }
		public CType CType { get; set; }
		public LinkedList<Comparator> Comparators { get; set; } = new LinkedList<Comparator>();

		public Condition(uint id, CType ctype)
		{
			ID = id;
			CType = ctype;
		}
	}

	public struct Comparator
	{
		public static string Validate(string name)
		{
			name = name.ToLowerInvariant();

			switch (name)
			{
				case "equ":
				case "neq":
				case "gtr":
				case "lss":
				case "geq":
				case "leq":
					return name;
			}

			throw new FormatException("No such comparator found.");
		}
		public static string Operator(string name)
		{
			switch (name)
			{
				case "equ": return "=";
				case "neq": return "≠";
				case "gtr": return ">";
				case "lss": return "<";
				case "geq": return "≥";
				case "leq": return "≤";
			}

			throw new FormatException("No such comparator found.");
		}

		public string Name { get; }
		public double Value { get; set; }

		public Comparator(string name, double value = 0)
		{
			Name = Validate(name);
			Value = value;
		}

		public string Operator() { return Operator(Name); }

		public override string ToString()
		{
			return string.Format("{0} {1:F2}", Operator(), Value);
		}
	}

	public class Write
	{
		public uint ID { get; set; }
		public CType CType { get; set; }
		public IValue Value { get; set; }

		public Write(uint id, CType ctype, IValue value)
		{
			ID = id;
			CType = ctype;
			Value = value ?? new DigitalState(false);
		}

		public void ToXml(StringBuilder builder)
		{

		}
	}
}
