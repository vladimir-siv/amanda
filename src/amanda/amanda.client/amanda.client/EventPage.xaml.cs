using System;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Communication;
using amanda.client.Models.Components;
using amanda.client.ViewModels;
using amanda.client.Views;
using amanda.client.Extensions;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class EventPage : ContentPage
	{
		private Label CreateLabel(string text)
		{
			return CreateLabel(text, (Color)Resources["TextColor"]);
		}
		private Label CreateLabel(string text, Color textColor)
		{
			// TODO: Create a label pool.
			return new Label
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				HorizontalTextAlignment = TextAlignment.Center,
				VerticalTextAlignment = TextAlignment.Center,
				Text = text,

				BackgroundColor = (Color)Resources["BackgroundColor"],
				TextColor = textColor,
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}
		private Label CreateLabel(FormattedString text)
		{
			// TODO: Create a label pool.
			return new Label
			{
				HorizontalOptions = LayoutOptions.FillAndExpand,
				HorizontalTextAlignment = TextAlignment.Center,
				VerticalTextAlignment = TextAlignment.Center,
				FormattedText = text,

				BackgroundColor = (Color)Resources["BackgroundColor"],
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}
		private Span CreateSpan(string text)
		{
			return CreateSpan(text, (Color)Resources["TextColor"]);
		}
		private Span CreateSpan(string text, Color textColor)
		{
			// TODO: Create a span pool.
			return new Span
			{
				Text = text,
				BackgroundColor = (Color)Resources["BackgroundColor"],
				TextColor = textColor,
				FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
				FontSize = Resources["FontSize"].PlatformResolve<double>()
			};
		}

		private Frame CreateWrapper(View content, Color borderColor)
		{
			return new Frame
			{
				CornerRadius = 0,
				BackgroundColor = (Color)Resources["BackgroundColor"],
				Margin = new Thickness(10, 0, 10, 0),
				Padding = new Thickness(1, 11, 1, 11),
				BorderColor = borderColor,
				Content = content
			};
		}
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
		private StackView CreateView()
		{
			// TODO: Create a StackView pool.
			return new StackView
			{
				Selectable = false,
				Margin = new Thickness(10, 0, 10, 0)
			};
		}

		public EventPage(EventViewModel vm)
		{
			InitializeComponent();
			BindingContext = vm;
		}

		protected override void OnAppearing()
		{
			base.OnAppearing();

			EventViewModel vm = BindingContext as EventViewModel;
			if (vm != null)
			{
				vm.EventChanged += OnEventChanged;
				vm.EventDeleting += OnEventDeleting;
				vm.EventDeleted += OnEventDeleted;
				OnEventChanged(vm, EventArgs.Empty);
			}
		}

		protected override void OnDisappearing()
		{
			EventViewModel vm = BindingContext as EventViewModel;
			if (vm != null)
			{
				vm.EventDeleted -= OnEventDeleted;
				vm.EventDeleting -= OnEventDeleting;
				vm.EventChanged -= OnEventChanged;
			}

			base.OnDisappearing();
		}

		private void OnEventChanged(object sender, EventArgs e)
		{
			// TODO: Optimize this.

			var vm = BindingContext as EventViewModel;
			if (vm == null) throw new ApplicationException("Unexpected error occured. Please, restart the application.");

			Requirements.Clear();

			foreach (var req in vm.Event.Requirements)
			{
				var ReqView = CreateView();

				Requirements.Add(CreateSubheader("Requirement"));
				Requirements.Add(CreateLine());
				Requirements.Add(CreateWrapper(ReqView, (Color)Resources["SubheadingColor"]));

				foreach (var condition in req.Conditions)
				{
					var component = RemoteDevice.FindComponent(condition.ID, condition.CType);
					var desc = component?.Description ?? string.Empty;
					var subh = desc + "[" + condition.ID + ":" + condition.CType.AsProtocolString() + "]";

					var CondView = CreateView();

					ReqView.Add(CreateSubheader(subh));
					ReqView.Add(CreateLine());
					ReqView.Add(CondView);

					foreach (var cmp in condition.Comparators)
					{
						Label label = null;
						
						if (component != null)
						{
							var value = component.Value.Clone();
							value.Write(cmp.Value.ToString());
							var displayed = value.Display();

							label = CreateLabel
							(
								new FormattedString
								{
									Spans =
									{
										CreateSpan(cmp.Operator() + ' '),
										CreateSpan(displayed.Value, displayed.Color)
									}
								}
							);
						}
						else label = CreateLabel(cmp.ToString());
						
						if (label != null) CondView.Add(label);
					}
				}
			}

			RaiseActions.Clear();
			ExpireActions.Clear();

			foreach (var write in vm.Event.Raise)
			{
				var desc = RemoteDevice.FindComponent(write.ID, write.CType)?.Description ?? string.Empty;
				var display = write.Value.Display();
				var text = desc + "[" + write.ID + ":" + write.CType.AsProtocolString() + "] → " + display.Value;
				var label = CreateLabel(text, display.Color);
				RaiseActions.Add(label);
			}

			foreach (var write in vm.Event.Expire)
			{
				var desc = RemoteDevice.FindComponent(write.ID, write.CType)?.Description ?? string.Empty;
				var display = write.Value.Display();
				var text = desc + "[" + write.ID + ":" + write.CType.AsProtocolString() + "] → " + display.Value;
				var label = CreateLabel(text, display.Color);
				ExpireActions.Add(label);
			}
		}

		private async Task<bool> OnEventDeleting()
		{
			var result = await DisplayActionSheet("Are you sure?", null, null, "Yes", "No");
			return result == "Yes";
		}
		private async void OnEventDeleted(object sender, ViewModelEventArgs e)
		{
			if (e.MType == ViewModelEventArgs.MessageType.Information && e.Message == "Success")
			{
				try
				{
					var xml_scan = await RemoteDevice.Send(Protocol.ScanEvents);
					await RemoteDevice.LoadEvents(xml_scan);
				}
				catch { }

				await DisplayAlert("Success", "The event was successfully deleted.", "OK");
				await Navigation.PopAsync();
			}
			else await DisplayAlert("Error", e.Message, "OK");
		}
	}
}
