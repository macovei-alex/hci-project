using API.DTOs;
using Microsoft.AspNetCore.Mvc;
using System.IO.Ports;

namespace API.Controllers
{
	[ApiController]
	[Route("api/intensity")]
	public class IntensityController : ControllerBase
	{
		private static SerialPort _serialPort;

		public IntensityController()
		{
			if (_serialPort == null)
			{
				_serialPort = new SerialPort("COM6", 9600);
				_serialPort.Open();
			}
		}

		[HttpPost("")]
		public IActionResult PostMessage([FromBody] IncreaseMessage requestBody)
		{
			if (string.IsNullOrEmpty(requestBody.Side))
			{
				return BadRequest("Message cannot be empty");
			}

			if (requestBody.Side != "right" && requestBody.Side != "left")
			{
				return BadRequest("Invalid side");
			}

			try
			{
				while (_serialPort.BytesToRead > 0)
				{
					_serialPort.ReadByte();
				}

				string message = requestBody.Side == "left" ? "l" : "r";
				message += requestBody.Increase ? "i" : "d";

				_serialPort.WriteLine(message);

				while (_serialPort.BytesToRead == 0) ;
				string received = _serialPort.ReadLine();

				Console.WriteLine(received);

				string[] split = received.Split(" ");
				if (split.Length != 3)
				{
					return StatusCode(500, "Invalid response");
				}

				return Ok(new Interval(float.Parse(split[1]), float.Parse(split[2])));
			}
			catch (Exception ex)
			{
				return StatusCode(500, $"Error: {ex.Message}");
			}
		}
	}

}
