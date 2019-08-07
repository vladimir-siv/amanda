using System;
using System.Net.Http;
using System.Collections.Generic;
using amanda.client.ViewModels;

namespace amanda.client
{
    public static class Dependency
    {
		private static Dictionary<string, object> context = new Dictionary<string, object>();
		private static Dictionary<Type, object> typed_context = new Dictionary<Type, object>();

		public static void Inject()
		{
			Inject(new HttpClient { Timeout = TimeSpan.FromSeconds(10) });

			ConnectionEstablishmentViewModel.Inject();
		}

		public static void Inject<T>(string name, T obj)
		{
			context.Add(name, obj);
		}
		public static T Resolve<T>(string name)
		{
			return (T)context[name];
		}

		public static void Inject<T>(T obj)
		{
			typed_context.Add(typeof(T), obj);
		}
		public static T Resolve<T>()
		{
			return (T)typed_context[typeof(T)];
		}
    }
}
