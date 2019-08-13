using System;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;
using amanda.client.Infrastructure.Measuring;
using amanda.client.Models.Components;
using amanda.client.Models.Events;
using amanda.client.ViewModels;
using amanda.client.Views;
using amanda.client.Extensions;

namespace amanda.client
{
	using Condition = Models.Events.Condition;

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
			bool success = false;
			Task alert = null;

			try
			{
				btnCreate.IsEnabled = false;
				IsCreating = true;

				var event_xml = await Task.Run(() =>
				{
					var Event = new Event(0, Name);

					for (int i = 0; i < Requirements.Count; ++i)
					{
						var ReqView = (StackView)((Frame)((StackLayout)Requirements[i]).Children[2]).Content;

						var req = new Requirement();
						Event.Requirements.AddLast(req);

						for (int j = 0; j < ReqView.Count; ++j)
						{
							var CondViewStack = (StackLayout)((Frame)((StackLayout)ReqView[j]).Children[2]).Content;
							var CondSensor = (Picker)CondViewStack.Children[0];
							var CondView = (StackView)CondViewStack.Children[1];

							var sensor = (ComponentViewModel)CondSensor.SelectedItem;
							var cond = new Condition(sensor.ID, sensor.CType.AsCType());
							req.Conditions.AddLast(cond);

							for (int k = 0; k < CondView.Count; ++k)
							{
								var CmpViewStack = (StackLayout)CondView[k];
								var cmp = (string)((Picker)CmpViewStack.Children[0]).SelectedItem;
								var val = ((Entry)CmpViewStack.Children[1]).Text;

								cond.Comparators.AddLast(new Comparator(Comparator.ToName(cmp), val.ValueConversion()));
							}
						}
					}

					for (int i = 0; i < RaiseActions.Count; ++i)
					{
						var actionStack = (StackLayout)((Frame)RaiseActions[i]).Content;
						var element = (ComponentViewModel)((Picker)actionStack.Children[0]).SelectedItem;

						var valueView = actionStack.Children[1];
						IValue value = null;

						if (valueView == null) continue;
						else if (valueView is Picker)
						{
							value = DigitalState.FromString((string)((Picker)valueView).SelectedItem);
						}
						else if (valueView is StackLayout)
						{
							var valueStack = (StackLayout)valueView;
							var analogValue = ((Entry)valueStack.Children[0]).Text;
							var unit = (string)((Picker)valueStack.Children[0]).SelectedItem;
							value = AnalogValue.FromString(analogValue + ' ' + unit);
						}
						else continue;
						
						var write = new Write(element.ID, element.CType.AsCType(), value);
						Event.Raise.AddLast(write);
					}

					for (int i = 0; i < ExpireActions.Count; ++i)
					{
						var actionStack = (StackLayout)((Frame)ExpireActions[i]).Content;
						var element = (ComponentViewModel)((Picker)actionStack.Children[0]).SelectedItem;

						var valueView = actionStack.Children[1];
						IValue value = null;

						if (valueView == null) continue;
						else if (valueView is Picker)
						{
							value = DigitalState.FromString((string)((Picker)valueView).SelectedItem);
						}
						else if (valueView is StackLayout)
						{
							var valueStack = (StackLayout)valueView;
							var analogValue = ((Entry)valueStack.Children[0]).Text;
							var unit = (string)((Picker)valueStack.Children[0]).SelectedItem;
							value = AnalogValue.FromString(analogValue + ' ' + unit);
						}
						else continue;
						
						var write = new Write(element.ID, element.CType.AsCType(), value);
						Event.Expire.AddLast(write);
					}

					return Protocol.CreateEvent(Event);
				});

				var response = await RemoteDevice.Send(event_xml);
				if (success = response == Protocol.ActionSuccess) alert = DisplayAlert("Success", "Event was successfully created!", "OK");
				else alert = DisplayAlert("Error", "The device could not create the event.", "OK");
			}
			catch (Exception ex) { alert = DisplayAlert("Error", "Could not create the event. Reason:\r\n" + ex.Message, "OK"); }
			finally
			{
				IsCreating = false;

				if (success)
				{
					try
					{
						var xml_scan = await RemoteDevice.Send(Protocol.ScanEvents);
						await RemoteDevice.LoadEvents(xml_scan);
					}
					catch { }
				}
				else btnCreate.IsEnabled = true;

				if (alert != null) await alert;
				if (success) await Navigation.PopAsync();
			}
		}
	}
}
