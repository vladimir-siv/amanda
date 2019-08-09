using System;

namespace amanda.client.Infrastructure.Measuring
{
	public interface IValue : IEquatable<IValue>
	{
		DisplayedValue Display();
		void Write(string value);
		event EventHandler<EventArgs> OnValueChange;
	}
}
