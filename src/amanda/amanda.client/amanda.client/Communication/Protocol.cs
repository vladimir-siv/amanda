namespace amanda.client.Communication
{
    public class Protocol
    {
		public static readonly string HelloMessage = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/hello\"></action>";
		public static readonly string HelloReply = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><message>Hello World.</message>";

	}
}
