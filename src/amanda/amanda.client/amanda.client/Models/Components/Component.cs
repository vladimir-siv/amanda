using System;
using System.Collections.Generic;
using amanda.client.Infrastructure;
using amanda.client.Infrastructure.Measuring;

namespace amanda.client.Models.Components
{
	[Flags] public enum CType { None = 0, Digital = 1, Analog = 2, Sensor = 4, Element = 8 }

	public static class ComponentTypes
	{
		public static readonly CType DigitalSensor = CType.Digital | CType.Sensor;
		public static readonly CType AnalogSensor = CType.Analog | CType.Sensor;
		public static readonly CType DigitalElement = CType.Digital | CType.Element;
		public static readonly CType AnalogElement = CType.Analog | CType.Element;

		public static CType AsCType(this string name)
		{
			switch (name)
			{
				case "DS": return DigitalSensor;
				case "AS": return AnalogSensor;
				case "DE": return DigitalElement;
				case "AE": return AnalogElement;
			}

			return CType.None;
		}
		public static string AsProtocolString(this CType ctype)
		{
			switch ((int)ctype)
			{
				case 0: return "";
				case 1: return "D";
				case 2: return "A";
				case 3: return "DA";
				case 4: return "S";
				case 5: return "DS";
				case 6: return "AS";
				case 7: return "DAS";
				case 8: return "E";
				case 9: return "DE";
				case 10: return "AE";
				case 11: return "DAE";
				case 12: return "SE";
				case 13: return "DSE";
				case 14: return "ASE";
				case 15: return "DASE";
				
				default: return string.Empty;
			}
		}

		public static bool Is(this IComponent component, CType ctype)
		{
			CType this_type = component.CType;
			CType ty = (CType)(((int)this_type) ^ ((int)ctype));
			return this_type != CType.None && ty == CType.None;
		}
		public static bool IsAny(this IComponent component, CType ctype)
		{
			CType this_type = component.CType;
			CType ty = (CType)(((int)this_type) & ((int)ctype));
			return this_type != CType.None && ty != CType.None;
		}
	}

	public interface IComponent
	{
		uint ID { get; }
		CType CType { get; }
		string Description { get; }
		IEnumerable<string> Commands { get; }
		IValue Value { get; }
	}

	public sealed class Component : IComponent
	{
		public uint ID { get; set; } = 0;
		public CType CType { get; set; } = CType.None;
		public string Description { get; set; } = string.Empty;
		public IEnumerable<string> Commands { get; set; } = null;
		public IValue Value { get; set; }

		public Component(uint id, CType ctype, string description, IEnumerable<string> commands, IValue initialValue = null)
		{
			ID = id;
			CType = ctype;
			Description = description;
			Commands = commands;
			Value = initialValue;
		}
	}
}
