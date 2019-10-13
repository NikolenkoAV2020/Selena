// SelenaTimer.h
// Walter Artyomenko, September 2019
//

class ZSelenaTimer {
public:

protected:
	struct timespec spec;
	struct tm       timeinfo;
	double          t;

public:
	ZSelenaTimer() {}
	~ZSelenaTimer() {}

	void Start(double dt) {
		t = dt;
		LocalTimeInfo(&timeinfo, &spec);
	}

	bool IsTimeUp() {
		struct timespec s;
		struct tm       ti;
		LocalTimeInfo(&ti, &s);
		double tt = (double)(s.tv_nsec - spec.tv_nsec) / 1e9
			+ (double)(s.tv_sec - spec.tv_sec);
		if (tt > t) {
			spec = s;
			timeinfo = ti;
			return true;
		}
		return false;
	}
};
