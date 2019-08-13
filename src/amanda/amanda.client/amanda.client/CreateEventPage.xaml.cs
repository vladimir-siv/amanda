using System;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;
using amanda.client.Infrastructure.Measuring;
using amanda.client.Models.Components;
using amanda.client.ViewModels;
using amanda.client.Views;
using amanda.client.Extensions;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class CreateEventPage : ContentPage
	{
		// TODO: Pool all view creators.

		private Label CreateSubheader(string text)
		{
			// TODO: Create a label pool.
			return new Label
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				Text = text,

				Margin = new Thickness(10, 0, 10, 0),
				BackgroundColor = (Color)Resources["BackgroundColor"],
				TextColor = (Color)Resources["SubheadingColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}
		private BoxView CreateLine()
		{
			// TODO: Create a BoxView pool.
			return new BoxView
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				HeightRequest = 1,
				Margin = new Thickness(10, 5, 10, 5),
				Color = (Color)Resources["SubheadingColor"]
			};
		}

		private View CreateAnalogDisplay()
		{
			var value = new Entry
			{
				HorizontalOptions = LayoutOptions.CenterAndExpand,
				Placeholder = "Analog Value",

				HorizontalTextAlignment = TextAlignment.Center,
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
			var unit = new Picker
			{
				ItemsSource = new string[]
				{
					"n",
					"V",
					"%",
					"lux",
					"*C",
					"ms"
				},
				SelectedIndex = 1,
				
				HorizontalOptions = LayoutOptions.CenterAndExpand,
				Margin = new Thickness(10, 0, 10, 0),
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>(),
				WidthRequest = Resources["ControlWidth"].PlatformResolve<double>()
			};

			return new StackLayout
			{
				Orientation = StackOrientation.Horizontal,
				HorizontalOptions = LayoutOptions.CenterAndExpand,
				Children = { value, unit }
			};
		}
		private View CreateDigitalDisplay(IValueDisplay displayer)
		{
			return new Picker
			{
				ItemsSource = new string[]
				{
					displayer.Display(true).Value,
					displayer.Display(false).Value
				},

				HorizontalOptions = LayoutOptions.CenterAndExpand,
				Margin = new Thickness(10, 0, 10, 0),
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>(),
				WidthRequest = Resources["ControlWidth"].PlatformResolve<double>()
			};
		}

		private View CreateComparator()
		{
			return new StackLayout
			{
				Orientation = StackOrientation.Horizontal,
				HorizontalOptions = LayoutOptions.CenterAndExpand,
				Children =
				{
					new Picker
					{
						ItemsSource = new string[]
						{
							"=",
							"≠",
							">",
							"<",
							"≥",
							"≤"
						},
						SelectedIndex = 1,
						
						HorizontalOptions = LayoutOptions.CenterAndExpand,
						Margin = new Thickness(10, 0, 10, 0),
						BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
						TextColor = (Color)Resources["TextColor"],
						FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
						FontSize = Resources["FontSize"].PlatformResolve<double>(),
						WidthRequest = Resources["ControlWidth"].PlatformResolve<double>()
					},
					new Entry
					{
						HorizontalOptions = LayoutOptions.CenterAndExpand,
						Placeholder = "Value",

						HorizontalTextAlignment = TextAlignment.Center,
						BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
						TextColor = (Color)Resources["TextColor"],
						FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
						FontSize = Resources["FontSize"].PlatformResolve<double>()
					}
				}
			};
		}
		private View CreateCondition()
		{
			var type = new Picker
			{
				ItemsSource = RemoteDevice.Sensors,
				ItemDisplayBinding = new Binding("Text"),

				HorizontalOptions = LayoutOptions.FillAndExpand,
				Margin = new Thickness(10, 0, 10, 0),
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};

			var layout = new StackLayout
			{
				Children =
				{
					type,
					new StackView
					{
						Generator = CreateComparator,
						Selectable = false,
						UserControllable = true,
						Margin = new Thickness()
					}
				}
			};

			return new StackLayout
			{
				Children =
				{
					CreateSubheader("Condition"),
					CreateLine(),
					new Frame
					{
						CornerRadius = 0,
						BackgroundColor = (Color)Resources["BackgroundColor"],
						Margin = new Thickness(10, 0, 10, 0),
						Padding = new Thickness(1, 11, 1, 11),
						BorderColor = (Color)Resources["SubheadingColor"],
						Content = layout
					}
				}
			};
		}
		private View CreateRequirement()
		{
			return new StackLayout
			{
				Children =
				{
					CreateSubheader("Requirement"),
					CreateLine(),
					new Frame
					{
						CornerRadius = 0,
						BackgroundColor = (Color)Resources["BackgroundColor"],
						Margin = new Thickness(10, 0, 10, 0),
						Padding = new Thickness(1, 11, 1, 11),
						BorderColor = (Color)Resources["SubheadingColor"],
						Content = new StackView
						{
							Generator = CreateCondition,
							Selectable = false,
							UserControllable = true,
							Margin = new Thickness(0)
						}
					}
				}
			};
		}
		private View CreateAction()
		{
			var type = new Picker
			{
				ItemsSource = RemoteDevice.Elements,
				ItemDisplayBinding = new Binding("Text"),

				HorizontalOptions = LayoutOptions.FillAndExpand,
				Margin = new Thickness(10, 0, 10, 0),
				BackgroundColor = (Color)Resources["ComponentBackgroundColor"],
				TextColor = (Color)Resources["TextColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};

			var layout = new StackLayout
			{
				Children =
				{
					type
				}
			};

			type.SelectedIndexChanged += (sender, e) =>
			{
				var selected = (ComponentViewModel)type.SelectedItem;

				if (layout.Children.Count > 2) // should always be false
				{
					layout.Children.Clear();
					layout.Children.Add(type);
				}

				if (layout.Children.Count == 2)
				{
					layout.Children.RemoveAt(1);
				}

				var ctype = selected.CType.AsCType();

				if (ctype.Is(CType.None)) return;
				else if (ctype.IsAny(CType.Analog)) layout.Children.Add(CreateAnalogDisplay());
				else if (ctype.IsAny(CType.Digital)) layout.Children.Add(CreateDigitalDisplay(((DigitalState)selected.Value).Displayer));
			};

			return new Frame
			{
				CornerRadius = 0,
				BackgroundColor = (Color)Resources["BackgroundColor"],
				Margin = new Thickness(0),
				Padding = new Thickness(1, 11, 1, 11),
				BorderColor = (Color)Resources["SubheadingColor"],
				Content = layout
			};
		}

		private string name;
		public string Name
		{
			get { return name; }
			set { name = value; OnPropertyChanged(); }
		}

		private string repeat;
		public string Repeat
		{
			get { return repeat; }
			set { repeat = value; OnPropertyChanged(); }
		}

		private Func<View> requirementGenerator;
		public Func<View> RequirementGenerator
		{
			get { return requirementGenerator; }
			set { requirementGenerator = value; OnPropertyChanged(); }
		}

		private Func<View> actionGenerator;
		public Func<View> ActionGenerator
		{
			get { return actionGenerator; }
			set { actionGenerator = value; OnPropertyChanged(); }
		}

		private bool isCreating;
		public bool IsCreating
		{
			get { return isCreating; }
			set { isCreating = value; OnPropertyChanged(); }
		}

		public CreateEventPage()
		{
			InitializeComponent();
			BindingContext = this;

			RequirementGenerator = CreateRequirement;
			ActionGenerator = CreateAction;
		}

		private async void OnCreateClick(object sender, EventArgs e)
		{
			
		}
	}
}
