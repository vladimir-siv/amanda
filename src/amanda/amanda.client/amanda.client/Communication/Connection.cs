namespace amanda.client.Communication
{
	public class Connection
	{
		public static void Inject() { Dependency.Inject(new Connection()); }

		public string Address { get; set; }
		public int Port { get; set; }

		private Connection() { }
		private Connection(string address, int port = 80) { Address = address; Port = port; }
	}
}
