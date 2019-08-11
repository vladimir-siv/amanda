using System;
using System.Collections.Generic;
using System.Text;

using amanda.client.Models.Components;
using amanda.client.Infrastructure.Measuring;

namespace amanda.client.Models.Events
{
    public class Event
    {
		public LinkedList<Requirement> Requirements { get; set; } = new LinkedList<Requirement>();
		public LinkedList<Write> Raise { get; set; } = new LinkedList<Write>();
		public LinkedList<Write> Expire { get; set; } = new LinkedList<Write>();
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
		public static string validate(string name)
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

		public string Name { get; }
		public double Value { get; set; }

		public Comparator(string name, double value = 0)
		{
			Name = validate(name);
			Value = value;
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
