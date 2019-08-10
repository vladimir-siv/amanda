using System.Collections.Generic;

namespace amanda.client.Synchronization
{
    public sealed class Atomic<T>
    {
		private object sync = new object();

		private T value;
		public T Value
		{
			get { lock (sync) return value; } // locking here possibly unnecessary
			set { lock (sync) this.value = value; } // and here as well
		}

		public Atomic(T initialValue = default(T)) { Value = initialValue; }

		public bool CompareAndSet(T refValue, T nextValue)
		{
			bool equal = false;

			lock (sync)
			{
				equal = EqualityComparer<T>.Default.Equals(value, refValue);
				value = nextValue;
			}

			return equal;
		}
    }
}
