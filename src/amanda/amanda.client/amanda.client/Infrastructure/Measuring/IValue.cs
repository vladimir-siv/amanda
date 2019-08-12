using System;

namespace amanda.client.Infrastructure.Measuring
{
	public interface IValue : IEquatable<IValue>
	{
		DisplayedValue Display();
		string Read();
		void Write(string value);
		IValue Clone();
		event EventHandler<EventArgs> OnValueChanged;
	}
}
