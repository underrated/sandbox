public class Task {

	String state = "start";
	String contextSwitch = "";
	int i;

	public void body() {
		switch (state) {
			case "start":

			System.out.println("Hello");

			System.out.println("Preparing to exit");

			// Context switch
			state = "first cs";
			return;
			case "first cs":
			
			System.out.println("Preparing to exit again");
			
			// Context switch
			state = "second cs";
			return;
			case "second cs":
			
			System.out.println("Let's try a loop");
			i = 0;
			// Start of loop
			case "startloop":

			System.out.println("Context Switch 1 in iteration "+ i);
			// Context switch 1
			state = "cs_in_loop1";
			return;
			case "cs_in_loop1":

			System.out.println("Context Switch 2 in iteration "+ i);
			// Context switch 2
			state = "cs_in_loop2";
			return;
			case "cs_in_loop2":

			// End of loop condition
			if(i<5) {
				i++;
				state = "startloop";
				return;
			}

			System.out.println("Re-entered again");

			System.out.println("Leaving for good...");

			state = "endoftask";
			case "endoftask":
			System.out.println("Task done...");
			return;

		}
	}

}
