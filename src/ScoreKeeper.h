// A class dedicated to keeping score.

enum ScoreKeeperJudgment{

	SKJ_NONE = 0, // no judgment.

	SKJ_W1 = 1, // Perfect / flashing Great
	SKJ_W2 = 2, // Great
	SKJ_W3 = 3, // Good
	SKJ_W4 = 4, // Bad
	SKJ_W5 = 5, // W5 is unused in beatmania.
	SKJ_MISS = 6, // Miss / Poor
	SKJ_HOLD_OK = 10, // OK, only used with DDR-style holds
	SKJ_HOLD_NG = 11, // NG

};

enum ScoreType{

	ST_SCORE = 1, // raindrop's 7K scoring type.

	ST_EX = 2, // EX score
	ST_DP = 3, // DP score

	ST_IIDX = 10, // IIDX score

	ST_OSU = 21, // osu!mania scoring
	ST_JB2 = 22, // jubeat^2 scoring

	ST_COMBO = 100, // current combo
	ST_MAX_COMBO = 101, // max combo
	ST_NOTES_HIT = 102, // total notes hit

};

enum PercentScoreType{

	PST_RANK = 1, // raindrop rank score

	PST_EX = 2, // EX score
	PST_NH = 3, // % notes hit
	PST_ACC = 4 // Accuracy

};

const double SCORE_MAX = 100000000;

enum LifeType{

	LT_GROOVE = 1, // Beatmania default score
	LT_SURVIVAL = 2, // Beatmania hard mode
	LT_EXHARD = 3, // Beatmania EX hard mode
	LT_DEATH = 4, // Sudden death mode

	LT_EASY = 5, // Beatmania easy mode

	LT_NORECOV = 6, // DDR no recov. mode
	LT_BATTERY = 7, // DDR battery mode.

};

class ScoreKeeper7K {

	public:

		ScoreKeeper7K();
		~ScoreKeeper7K();

		void setMaxNotes(int notes);
		int getMaxNotes();
		void setAccMin(int ms);
		void setAccMax(int ms);
		void setEX2(int ms);
		void setEX1(int ms);
		void setDP2(int ms);
		void setDP1(int ms);

		int getJudgmentCount(ScoreKeeperJudgment Judge);
		int getJudgmentWindow(ScoreKeeperJudgment judgment);
		std::string getHistogram();

		ScoreKeeperJudgment hitNote(int ms);
		void missNote(bool auto_hold_miss);

		double getAccCutoff();
		double getAccMax();

		int getScore(ScoreType score_type);
		float getPercentScore(PercentScoreType score_type);

		int getLifebarUnits(LifeType lifebar_unit_type);
		float getLifebarAmount(LifeType lifebar_amount_type);

		int getRank(); // returns a number from -9 to 9

		void reset();

	private:

		double score; // standard score.
		double sc_score;
		double sc_sc_score;

	/*
		Rank scoring
	*/

		long long rank_w1_count;
		long long rank_w2_count;
		long long rank_w3_count;

		long long rank_pts; // rank scoring

		void update_ranks(int ms);

		long long max_notes;

	/*
		BMS scoring
	*/

		long long ex_score;

		long long bms_combo;
		long long bms_combo_pts;
		long long bms_max_combo_pts;

		long long bms_dance_pts;
		long long bms_score;

		void update_bms(int ms, bool hit);
		void update_lr2(int ms, bool hit);

	/*
		osu!
	*/

		void set_osu_judgment(int ms, ScoreKeeperJudgment judgment);

	/*
		misc.
	*/

		long long notes_hit; // notes hit %.
		long long total_notes;


		long long dp_score; // DDR dance-polong long scoring
		long long dp_dp_score;

		long long combo;
		long long max_combo;

		double total_sqdev; // accuracy scoring
		double accuracy;

		double accuracy_percent(float ms);

		// lifebar data.

		double lifebar_groove;
		double lifebar_groove_increment;

		double lifebar_survival;
		double lifebar_survival_increment;

		double lifebar_exhard;
		double lifebar_exhard_increment;

		double lifebar_death;

		double lifebar_easy;
		double lifebar_easy_increment;

		// judgement information
		double judgement_time[9];
		double judgement_amt[9];

		double histogram[255]; // records from -127 to +127 ms.

		// no-recovery modes.
		double lifebar_battery;

		long long lifebar_battery_lives;

		// scoring parameters.
		long long ACC_MIN, ACC_MIN_SQ;
		long long ACC_MAX, ACC_MAX_SQ;
		long long ACC_CUTOFF;

		long long EX2, EX1;
		long long DP2, DP1;

};
