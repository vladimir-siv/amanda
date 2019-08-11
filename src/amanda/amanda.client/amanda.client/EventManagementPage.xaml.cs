using System;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

using amanda.client.Extensions;

namespace amanda.client
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class EventManagementPage : ContentPage
	{
		private bool isLoading;
		public bool IsLoading
		{
			get { return isLoading; }
			set { if (value == isLoading) return; isLoading = value; OnPropertyChanged(); }
		}

		private Func<View> labelGenerator;
		public Func<View> LabelGenerator
		{
			get { return labelGenerator; }
			set { labelGenerator = value; OnPropertyChanged(); }
		}

		public EventManagementPage()
		{
			InitializeComponent();
			BindingContext = this;

			EventView.IsVisible = true;
			
			LabelGenerator = () =>
			{
				return new Label
				{
					HorizontalOptions = LayoutOptions.FillAndExpand,
					HorizontalTextAlignment = TextAlignment.Center,
					VerticalTextAlignment = TextAlignment.Center,
					Text = "Test",

					BackgroundColor = (Color)Resources["BackgroundColor"],
					TextColor = (Color)Resources["TextColor"],
					FontFamily = Resources["FontFamily"].PlatformResolve<string>(),
					FontSize = Resources["FontSize"].PlatformResolve<double>()
				};
			};

			Label lbl;

			lbl = (Label)LabelGenerator();
			lbl.Text = "Whatever1";
			Events.Add(lbl);

			lbl = (Label)LabelGenerator();
			lbl.Text = "Whatever2";
			Events.Add(lbl);

			lbl = (Label)LabelGenerator();
			lbl.Text = "Whatever3";
			Events.Add(lbl);

			lbl = (Label)LabelGenerator();
			lbl.Text = "Whatever4";
			Events.Add(lbl);


		}
	}
}
