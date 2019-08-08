using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;
using System.Reflection;
using System.Threading.Tasks;

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

		public void Attach<TViewModelEventArgs>(string dispatcher, ViewModelEventHandler<TViewModelEventArgs> callback) where TViewModelEventArgs : ViewModelEventArgs
		{
			// [WARNING] Reflection used. May be slow.

			dispatcher = dispatcher ?? throw new ArgumentNullException("Dispatcher cannot be null.");
			callback = callback ?? throw new ArgumentNullException("Callback cannot be null.");

			Type ty = GetType();

			EventInfo e = ty.GetEvent(dispatcher, BindingFlags.Instance | BindingFlags.Public);

			if (e.MemberType != MemberTypes.Event) throw new SystemException("\'" + dispatcher + "\' is not a valid dispatcher.");

			if (e == null) throw new DispatcherNotFoundException("Could not find dispatcher \'" + dispatcher + "\".");

			if (callback.GetType() != e.EventHandlerType) throw new DispatcherTypeMismatchException("Dispatcher type does not match with the callback type.");

			e.AddEventHandler(this, callback);
		}
		public void Detach<TViewModelEventArgs>(string dispatcher, ViewModelEventHandler<TViewModelEventArgs> callback) where TViewModelEventArgs : ViewModelEventArgs
		{
			// [WARNING] Reflection used. May be slow.

			dispatcher = dispatcher ?? throw new ArgumentNullException("Dispatcher cannot be null.");
			callback = callback ?? throw new ArgumentNullException("Callback cannot be null.");

			Type ty = GetType();

			EventInfo e = ty.GetEvent(dispatcher, BindingFlags.Instance | BindingFlags.Public);

			if (e.MemberType != MemberTypes.Event) throw new SystemException("\'" + dispatcher + "\' is not a valid dispatcher.");

			if (e == null) throw new DispatcherNotFoundException("Could not find dispatcher \'" + dispatcher + "\".");

			if (callback.GetType() != e.EventHandlerType) throw new DispatcherTypeMismatchException("Dispatcher type does not match with the callback type.");

			e.RemoveEventHandler(this, callback);
		}

		protected async Task DispatchAsync<TViewModelEventArgs>(ViewModelEventHandler<TViewModelEventArgs> dispatcher, TViewModelEventArgs e) where TViewModelEventArgs : ViewModelEventArgs
		{
			if (dispatcher == null) return;

			Delegate[] invocation_list = dispatcher.GetInvocationList();
			Task[] callbacks = new Task[invocation_list.Length];

			for (int i = 0; i < invocation_list.Length; ++i)
			{
				callbacks[i] = ((ViewModelEventHandler<TViewModelEventArgs>)invocation_list[i]).Invoke(this, e);
			}

			await Task.WhenAll(callbacks);
		}
		protected async Task Dispatch<TViewModelEventArgs>(ViewModelEventHandler<TViewModelEventArgs> dispatcher, TViewModelEventArgs e) where TViewModelEventArgs : ViewModelEventArgs
		{
			if (dispatcher == null) return;

			Delegate[] invocation_list = dispatcher.GetInvocationList();
			
			for (int i = 0; i < invocation_list.Length; ++i)
			{
				await ((ViewModelEventHandler<TViewModelEventArgs>)invocation_list[i]).Invoke(this, e);
			}
		}
	}

	public class ViewModelEventArgs : EventArgs
	{
		public enum MessageType { None = 0, Information = 1, Warning = 2, Error = 4 }

		public static ViewModelEventArgs Information(string message) { return new ViewModelEventArgs(message, MessageType.Information); }
		public static ViewModelEventArgs Warning(string message) { return new ViewModelEventArgs(message, MessageType.Warning); }
		public static ViewModelEventArgs Error(string message) { return new ViewModelEventArgs(message, MessageType.Error); }

		public static readonly new ViewModelEventArgs Empty = new ViewModelEventArgs();

		public readonly string Name = string.Empty;
		public readonly string Message = string.Empty;
		public readonly MessageType MType = MessageType.None;

		public ViewModelEventArgs() : this(string.Empty) { }
		public ViewModelEventArgs(string name) : this(name, string.Empty) { }
		public ViewModelEventArgs(string name, string message) : this(name, message, MessageType.None) { }
		public ViewModelEventArgs(string message, MessageType mtype) : this(string.Empty, message, mtype) { }
		public ViewModelEventArgs(string name, string message, MessageType mtype) { Name = name ?? string.Empty; Message = message ?? string.Empty; MType = mtype; }
	}

	public delegate Task ViewModelEventHandler<TViewModelEventArgs>(ViewModel sender, TViewModelEventArgs e) where TViewModelEventArgs : ViewModelEventArgs;

	public class DispatcherNotFoundException : Exception
	{
		public DispatcherNotFoundException() : base() { }
		public DispatcherNotFoundException(string message) : base(message) { }
		public DispatcherNotFoundException(string message, Exception innerException) : base(message, innerException) { }
		protected DispatcherNotFoundException(SerializationInfo info, StreamingContext context) : base (info, context) { }
	}

	public class DispatcherTypeMismatchException : Exception
	{
		public DispatcherTypeMismatchException() : base() { }
		public DispatcherTypeMismatchException(string message) : base(message) { }
		public DispatcherTypeMismatchException(string message, Exception innerException) : base(message, innerException) { }
		protected DispatcherTypeMismatchException(SerializationInfo info, StreamingContext context) : base(info, context) { }
	}
}
