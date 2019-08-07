using System.Net.Http;
using System.Text;

namespace amanda.client.Communication
{
    public class Protocol
    {
		public static readonly StringContent HelloMessage = new StringContent("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/hello\"></action>", Encoding.UTF8, "application/xml");
		public static readonly string HelloReply = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><message>Hello World.</message>";

	}
}
