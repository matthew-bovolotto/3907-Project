#define RED 2
#define GREEN 1
#define BLUE 0

class frame_display{
		public:
			frame_display();
			void disp_frame(unsigned long[]);
			void fade(int time);

		private:
			void clock(short int i);
};
