using Xamarin.Forms;

namespace amanda.client.Extensions
{
    public static class Extensions
    {
		public static T Resolve<T>(this OnPlatform<T> value)
		{
			if (value == null) return default(T);

			switch (Device.RuntimePlatform)
			{
				case Device.Android: return value.Android;
				case Device.iOS: return value.iOS;
				case Device.UWP: return value.WinPhone;
				default: return value.Default;
			}
		}

		public static T PlatformResolve<T>(this object value)
		{
			return Resolve(value as OnPlatform<T>);
		}
    }
}
