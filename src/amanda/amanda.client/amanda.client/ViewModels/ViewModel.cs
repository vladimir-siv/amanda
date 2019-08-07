using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace amanda.client.ViewModels
{
    public abstract class ViewModel : INotifyPropertyChanged
	{
		private static Dictionary<string, ViewModel> resolver = new Dictionary<string, ViewModel>();
		protected static void Inject(string name, ViewModel vm) { resolver.Add(name, vm); }
		public static ViewModel Resolve(string name) { return resolver[name]; }

		public event PropertyChangedEventHandler PropertyChanged;

		protected ViewModel() { }
		protected ViewModel(string name) { Inject(name, this); }

		protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = "")
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
		}
	}
}
