namespace API.DTOs
{
	public class Interval
	{
		public float Min { get; set; }
		public float Max { get; set; }

		public Interval(float min, float max)
		{
			Min = min;
			Max = max;
		}
	}
}
