using System;
using System.Net.Http;
using System.Collections.Generic;
using System.Threading.Tasks;

using amanda.client.ViewModels;
using amanda.client.Communication;

namespace amanda.client
{
    public static class Dependency
    {
		private static Dictionary<string, object> context = new Dictionary<string, object>();
		private static Dictionary<Type, object> typed_context = new Dictionary<Type, object>();

		public static async Task Inject()
		{
			Inject(new HttpClient { Timeout = TimeSpan.FromSeconds(10) });

			Connection.Inject();
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
