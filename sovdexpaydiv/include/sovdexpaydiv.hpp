#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>


using namespace eosio;



CONTRACT sovdexpaydiv : public contract {
  public:
    using contract::contract;

    //initialize system-----------------------------
    [[eosio::action]] 
    void initialize(name contract, asset clubstaked);

    [[eosio::action]] 
    void intstaker(name user, asset svxstaked, int laststaketime);

    //listen to mining 
    [[eosio::on_notify("sovdexrelays::minereceipt")]] void paydiv(name user, eosio::asset sovburned, asset minepool);

    //listen to transfers to get paid out
    [[eosio::on_notify("goldgoldgold::transfer")]] void insertgold(name from, name to, asset quantity, std::string memo);
    [[eosio::on_notify("btcbtcbtcbtc::transfer")]] void insertbtc(name from, name to, asset quantity, std::string memo);
    [[eosio::on_notify("svxmintofeos::transfer")]] void insertsvx(name from, name to, asset quantity, std::string memo);
    
    //listen to stake / unstake
    [[eosio::on_notify("svxmintofeos::stake")]] void setstake(name account, asset value);
    [[eosio::on_notify("svxmintofeos::unstake")]] void setunstake(name account, asset value);
    
    
    
    TABLE staketable { 
      
      name      staker;
      asset     svxstaked;
      uint32_t  staketime;
      
      auto primary_key() const { return staker.value; }
    };
    typedef multi_index<name("staketable"), staketable> stake_table;



    TABLE svxstakestat {
      
      asset      clubstaked;
     
      
      auto primary_key() const { return clubstaked.symbol.code().raw(); }
    };
    typedef multi_index<name("svxstakestat"), svxstakestat> svxstake_stat;



    TABLE queuetable { 
      
      name       contract;
      
      //asset      currenttoken; // current token being paid out 
      name       currentpayee;

      asset      remainingpay_gold; // divs loaded in queue
      asset      remainingpay_btc;
      asset      remainingpay_svx;

      asset      startpay_gold; // amount of divs at the start of the round, should be calc from LR*queue
      asset      startpay_btc;
      asset      startpay_svx;

      int        loadingratio; //determines the rate at which divs are loaded from the queue to the active round

      asset      clubstakestart; //the amount of SVX staked in 777 club at the start of the active round
      
      int        payoutstarttime; //the time of the start if the rounf 
      
      
      
      auto primary_key() const { return contract.value; }

    };
    typedef multi_index<name("queuetable"), queuetable> queue_table;





     struct [[eosio::table]] account {
            asset    balance;
            asset    storebalance;
            asset    svxpower;
            asset    unstaking;
            uint32_t  unstake_time;
            

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };
  
        typedef eosio::multi_index< "accounts"_n, account > accounts;

        
        static asset get_stored_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
         {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.storebalance;

//----------------------------------------------------------------------------------------------------------------

    name get_current_payee(){

        name payee; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        payee = existing->currentpayee;

        return payee;

    }
//------------------ remaining pay getters --------------------------------------------------------//

    asset get_remaining_pay_gold(){

        asset rp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        rp = existing->remainingpay_gold;

        return rp;


    }
    asset get_remaining_pay_btc(){

        asset rp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        rp = existing->remainingpay_btc;

        return rp;


    }
    asset get_remaining_pay_svx(){

        asset rp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        rp = existing->remainingpay_svx;

        return rp;


    }
//----------------------------------------startingpay gettters --------------------------//
    asset get_starting_pay_gold(){

        asset sp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        sp = existing->startpay_gold;

        return sp;


    }
    asset get_starting_pay_btc(){

        asset sp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        sp = existing->startpay_btc;

        return sp;


    }
    asset get_starting_pay_svx(){

        asset sp; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        sp = existing->startpay_svx;

        return sp;

//-------------------------------------------------------------------------------------------------------------
    }

    asset get_club_stake_start(){

        asset css; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        css = existing->clubstakestart;

        return css;


    }


    uint32_t get_payout_start_time(){

        uint32_t pst; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        pst = existing->payoutstarttime;

        return pst;




    }


    asset get_userstake(name user){

        asset stake;

        stake_table staketable(get_self(), get_self().value);
        auto existing = staketable.find(user.value);

        stake = existing->svxstaked;

        return stake;

    }


    uint32_t get_userstaketime(name user){

        uint32_t staketime;

        stake_table staketable(get_self(), get_self().value);
        auto existing = staketable.find(user.value);

        staketime = existing->staketime;

        return staketime;

    }

    double get_paying_ratio(){

        name user = get_current_payee();
        asset stake = get_userstake(user);
        asset css = get_club_stake_start();


        double payout_frac = (double(stake.amount) / double(css.amount))*100;

        return payout_frac;
    }


    void sendasset(name user, asset quantity, const std::string& memo){


      std::string sym =  quantity.symbol.code().to_string();

      if (quantity.amount < 0){

          return;
      }

/**
     if (sym == "PBTC"){
        
          action(permission_level{_self, "active"_n}, "TODOFILLCONTRACTHERE"_n, "transfer"_n, 
          std::make_tuple(get_self(), user, quantity, std::string("SVX send"))).send();
        
      }

      if (sym == "XAUT"){
        
          action(permission_level{_self, "active"_n}, "TODOFILLCONTRACTHERE"_n, "transfer"_n, 
          std::make_tuple(get_self(), user, quantity, std::string("EOS send"))).send();
        
      }
  **/
      
     if (sym == "SVX"){
        
          action(permission_level{_self, "active"_n}, "svxmintofeos"_n, "transfer"_n, 
          std::make_tuple(get_self(), user, quantity, std::string("SVX send"))).send();

    }

  }


  uint32_t get_loading_ratio(){

        uint32_t lr; 

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);

        lr = existing->loadingratio;

        return lr;

  }



  // SETTERS


  void update_queue(asset quantity){

        std::string tokensym = quantity.symbol.code().to_string();

        queue_table queuetable(get_self(), get_self().value);
        auto existing = queuetable.find(get_self().value);
        check( existing != queuetable.end(), "contract table not deployed" );
        const auto& st = *existing;

        if (tokensym == "PBTC"){


                queuetable.modify( st, same_payer, [&]( auto& s ) {
                    s.remainingpay_btc += quantity;
                });

        }

        if (tokensym == "XAUT"){


                queuetable.modify( st, same_payer, [&]( auto& s ) {
                    s.remainingpay_gold += quantity;
                });

        }

        if (tokensym == "SVX"){


                queuetable.modify( st, same_payer, [&]( auto& s ) {
                    s.remainingpay_svx += quantity;
                });

        }



  }

  void update_global_stake(name user, asset stakechange, uint32_t staketime){

   //1 case one, didnt have 777k, staked and still doesnt have 777k - edit nothing 
   //2 case two, had 777k, staked and now has even more - modify global, modify club table
   //3 case three didnt have 777k, but now has 777k - modify global stake, emplace club stake table
   
   
    //1 find svx stored

    asset stored = get_stored_balance(name{"svxmintofeos"}, user, symbol_code("SVX") );

    if (stored.amount < (777000*1000)){  //CASE1 CONSIDERED: NOW DOESNT HAVE 777K STAKED
        
        return;

    }

    stake_table staketable(get_self(), get_self().value);
    auto existing = staketable.find(user.value);
    const auto& st = *existing;


    svxstake_stat globalstake(get_self(),get_self().value);
    auto existing2 = globalstake.find(symbol_code("SVX"));
    const auto& st2 = *existing2;


    if (existing == staketable.end()){ //CASE 3 CONSIDERED: DIDNT HAVE 777K STAKED BUT NOW DOES

        staketable.emplace( get_self(), [&]( auto& s ) {

                        s.staker = user;
                        s.svxstaked = stored;
                        s.staketime = current_time_point().sec_since_epoch();

                  });
    
    }

    else { //CASE3 CONSIDERED: HAS 777K STAKED, STAKED EVEN MORE

        staketable.modify( st, same_payer, [&]( auto& s ) {

                        s.staker = user;
                        s.svxstaked = s.svxstaked + stakechange;
                        s.staketime = current_time_point().sec_since_epoch();

                  });


    }




  }

  void update_global_unstake(name user, asset stakechange, uint32_t staketime){

    

  }

  



 





};







