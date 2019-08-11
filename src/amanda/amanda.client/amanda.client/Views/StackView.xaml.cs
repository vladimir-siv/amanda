using System;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace amanda.client.Views
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class StackView : ContentView
	{
		private Color ViewBackgroundColor;
		private StackLayout ControlStackReference;
		private TapGestureRecognizer TapGesture;

		public static readonly BindableProperty GeneratorProperty = BindableProperty.Create
		(
			propertyName: "Generator",
			returnType: typeof(Func<View>),
			declaringType: typeof(StackView),
			defaultValue: null,
			defaultBindingMode: BindingMode.TwoWay
		);
		public Func<View> Generator
		{
			get => (Func<View>)GetValue(GeneratorProperty);
			set => SetValue(GeneratorProperty, value);
		}

		public static readonly BindableProperty SelectableProperty = BindableProperty.Create
		(
			propertyName: "Selectable",
			returnType: typeof(bool),
			declaringType: typeof(StackView),
			defaultValue: true,
			defaultBindingMode: BindingMode.TwoWay,
			propertyChanged: (bindable, oldValue, newValue) =>
			{
				var THIS = (StackView)bindable;
				var oldVal = (bool)oldValue;
				var newVal = (bool)newValue;

				if (!newVal) THIS.Selected = null;
			}
		);
		public bool Selectable
		{
			get => (bool)GetValue(SelectableProperty);
			set => SetValue(SelectableProperty, value);
		}

		public static readonly BindableProperty SelectedProperty = BindableProperty.Create
		(
			propertyName: "Selected",
			returnType: typeof(View),
			declaringType: typeof(StackView),
			defaultValue: null,
			defaultBindingMode: BindingMode.TwoWay,
			propertyChanging: (bindable, oldValue, newValue) =>
			{
				if (newValue == null) return;
				var THIS = (StackView)bindable;
				var newVal = (View)newValue;
				if (!THIS.ContentStack.Children.Contains(newVal))
					throw new ArgumentException("The StackView does not contain such view to be able to select it.");
			},
			propertyChanged: (bindable, oldValue, newValue) =>
			{
				var THIS = (StackView)bindable;

				var oldVal = oldValue as View;
				if (oldVal != null) oldVal.BackgroundColor = THIS.ViewBackgroundColor;

				var newVal = newValue as View;
				if (newVal != null)
				{
					THIS.ViewBackgroundColor = newVal.BackgroundColor;
					newVal.BackgroundColor = THIS.SelectedColor;
				}
			}
		);
		public View Selected
		{
			get => (View)GetValue(SelectedProperty);
			set => SetValue(SelectedProperty, value);
		}

		public static readonly BindableProperty SelectedColorProperty = BindableProperty.Create
		(
			propertyName: "SelectedColor",
			returnType: typeof(Color),
			declaringType: typeof(StackView),
			defaultValue: null,
			defaultBindingMode: BindingMode.TwoWay,
			propertyChanged: (bindable, oldValue, newValue) =>
			{
				var THIS = (StackView)bindable;
				if (THIS.Selected != null) THIS.Selected.BackgroundColor = (Color)newValue;
			}
		);
		public Color SelectedColor
		{
			get => (Color)GetValue(SelectedColorProperty);
			set => SetValue(SelectedColorProperty, value);
		}

		public static readonly BindableProperty UserControllableProperty = BindableProperty.Create
		(
			propertyName: "UserControllable",
			returnType: typeof(bool),
			declaringType: typeof(StackView),
			defaultValue: true,
			defaultBindingMode: BindingMode.TwoWay,
			propertyChanged: (bindable, oldValue, newValue) =>
			{
				var THIS = (StackView)bindable;
				var oldVal = (bool)oldValue;
				var newVal = (bool)newValue;

				if (THIS.ControlStackReference == null) throw new NullReferenceException("ControlStackReference is null");

				if (oldVal == newVal) return;

				bool controlsDisplayed = THIS.ViewStack.Children.Contains(THIS.ControlStackReference);

				if (newVal)
				{
					if (!controlsDisplayed)
						THIS.ViewStack.Children.Add(THIS.ControlStackReference);
				}
				else
				{
					if (controlsDisplayed)
						THIS.ViewStack.Children.Remove(THIS.ControlStackReference);
				}
			}
		);
		public bool UserControllable
		{
			get => (bool)GetValue(UserControllableProperty);
			set => SetValue(UserControllableProperty, value);
		}

		public event EventHandler<EventArgs> ItemTapped;

		public StackView()
		{
			InitializeComponent();
			ControlStackReference = (StackLayout)ViewStack.Children[1];
			TapGesture = new TapGestureRecognizer();
			TapGesture.Tapped += OnItemTap;
		}

		public void Add(View view)
		{
			if (ContentStack.Children.Contains(view)) return;
			ContentStack.Children.Add(view);
			view.GestureRecognizers.Add(TapGesture);
		}

		public void Remove(View view)
		{
			if (!ContentStack.Children.Contains(view)) return;
			ContentStack.Children.Remove(view);
			view.GestureRecognizers.Remove(TapGesture);
		}

		public void RemoveLast()
		{
			int index = ContentStack.Children.Count - 1;
			if (index < 0) return;
			var view = ContentStack.Children[index];
			ContentStack.Children.RemoveAt(index);
			view.GestureRecognizers.Remove(TapGesture);
		}

		private void OnAddTap(object sender, EventArgs e)
		{
			Add(Generator());
		}

		private void OnRemoveTap(object sender, EventArgs e)
		{
			RemoveLast();
		}

		private void OnItemTap(object sender, EventArgs e)
		{
			if (Selectable) Selected = (View)sender;
			ItemTapped?.Invoke(sender, e);
		}
	}
}
