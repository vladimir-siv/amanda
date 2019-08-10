using Xamarin.Forms;
using Xamarin.Forms.Xaml;

[assembly: XamlCompilation(XamlCompilationOptions.Compile)]
namespace amanda.client
{
	public partial class App : Application
	{
		public App()
		{
			InitializeComponent();
			
			MainPage = new NavigationPage(new ConnectPage());
		}

		protected async override void OnStart()
		{
			// Handle when your app starts
			await Dependency.Inject();
		}

		protected override void OnSleep()
		{
			// Handle when your app sleeps
		}

		protected override void OnResume()
		{
			// Handle when your app resumes
		}
	}
}
