using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace amanda.client.Views.Cells
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class MenuOptionCell : ViewCell
	{
		public static readonly BindableProperty TextProperty = BindableProperty.Create
		(
			propertyName: "Text",
			returnType: typeof(string),
			declaringType: typeof(MenuOptionCell),
			defaultValue: string.Empty,
			defaultBindingMode: BindingMode.TwoWay,
			propertyChanged: (bindable, oldValue, newValue) =>
			{
				var THIS = (MenuOptionCell)bindable;
				THIS.lblOptionText.Text = (string)newValue;
			}
		);
		public string Text => (string)GetValue(TextProperty);

		public MenuOptionCell()
		{
			InitializeComponent();
		}
	}
}
